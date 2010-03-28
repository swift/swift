#include "Swiften/Avatars/AvatarManager.h"

#include <boost/bind.hpp>

#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Elements/VCardUpdate.h"
#include "Swiften/Queries/Requests/GetVCardRequest.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/StringCodecs/Hexify.h"
#include "Swiften/Avatars/AvatarStorage.h"
#include "Swiften/MUC/MUCRegistry.h"

namespace Swift {

AvatarManager::AvatarManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, AvatarStorage* avatarStorage, MUCRegistry* mucRegistry) : stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), avatarStorage_(avatarStorage), mucRegistry_(mucRegistry) {
	stanzaChannel->onPresenceReceived.connect(boost::bind(&AvatarManager::handlePresenceReceived, this, _1));
}

AvatarManager::AvatarManager() {
	stanzaChannel_ = NULL;
	iqRouter_ = NULL;
	avatarStorage_ = NULL;
	mucRegistry_ = NULL;
}

AvatarManager::~AvatarManager() {

}

void AvatarManager::setMUCRegistry(MUCRegistry* mucRegistry) {
	mucRegistry_ = mucRegistry;
}

void AvatarManager::handlePresenceReceived(boost::shared_ptr<Presence> presence) {
	boost::shared_ptr<VCardUpdate> update = presence->getPayload<VCardUpdate>();
	if (!update) {
		return;
	}
	JID from = getAvatarJID(presence->getFrom());
	String& hash = avatarHashes_[from];
	if (hash != update->getPhotoHash()) {
		String newHash = update->getPhotoHash();
		if (avatarStorage_->hasAvatar(newHash)) {
			setAvatarHash(from, newHash);
		}
		else {
			boost::shared_ptr<GetVCardRequest> request(new GetVCardRequest(from, iqRouter_));
			request->onResponse.connect(boost::bind(&AvatarManager::handleVCardReceived, this, from, newHash, _1, _2));
			request->send();
		}
	}
}

void AvatarManager::handleVCardReceived(const JID& from, const String& promisedHash, boost::shared_ptr<VCard> vCard, const boost::optional<ErrorPayload>& error) {
	if (error) {
		// FIXME: What to do here?
		std::cerr << "Warning: " << from << ": Could not get vCard" << std::endl;
		return;
	}
	if (!vCard) {
		std::cerr << "Warning: " << from << ": null vcard payload" << std::endl;
		//FIXME: Why could this happen?
		return;
	}
	String realHash = Hexify::hexify(SHA1::getHash(vCard->getPhoto()));
	if (promisedHash != realHash) {
		std::cerr << "Warning: " << from << ": Got different vCard photo hash (" << promisedHash << " != " << realHash << ")" << std::endl;
	}
	avatarStorage_->addAvatar(realHash, vCard->getPhoto());
	setAvatarHash(from, realHash);
}

void AvatarManager::setAvatar(const JID& jid, const ByteArray& avatar) {
	String hash = Hexify::hexify(SHA1::getHash(avatar));
	avatarStorage_->addAvatar(hash, avatar);
	setAvatarHash(getAvatarJID(jid), hash);
}

void AvatarManager::setAvatarHash(const JID& from, const String& hash) {
	avatarHashes_[from] = hash;
	onAvatarChanged(from, hash);
}

String AvatarManager::getAvatarHash(const JID& jid) const {
	std::map<JID, String>::const_iterator i = avatarHashes_.find(getAvatarJID(jid));
	if (i != avatarHashes_.end()) {
		return i->second;
	}
	else {
		return "";
	}
}

boost::filesystem::path AvatarManager::getAvatarPath(const JID& jid) const {
	String hash = getAvatarHash(jid);
	if (!hash.isEmpty()) {
		return avatarStorage_->getAvatarPath(hash);
	}
	return boost::filesystem::path();
}

JID AvatarManager::getAvatarJID(const JID& jid) const {
	JID bareFrom = jid.toBare();
	return (mucRegistry_ && mucRegistry_->isMUC(bareFrom)) ? jid : bareFrom;
}


}
