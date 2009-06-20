#include "Swiften/Avatars/AvatarManager.h"

#include <boost/bind.hpp>

#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Elements/VCardUpdate.h"
#include "Swiften/Queries/Requests/GetVCardRequest.h"
#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {

AvatarManager::AvatarManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, AvatarStorage* avatarStorage) : stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), avatarStorage_(avatarStorage) {
	stanzaChannel->onPresenceReceived.connect(boost::bind(&AvatarManager::handlePresenceReceived, this, _1));
}

void AvatarManager::handlePresenceReceived(boost::shared_ptr<Presence> presence) {
	boost::shared_ptr<VCardUpdate> update = presence->getPayload<VCardUpdate>();
	if (!update) {
		return;
	}
	JID from = presence->getFrom().toBare();
	String& hash = avatarHashes_[from];
	if (hash != update->getPhotoHash()) {
		hash = update->getPhotoHash();
		if (!avatarStorage_->hasAvatar(hash)) {
			boost::shared_ptr<GetVCardRequest> request(new GetVCardRequest(from, iqRouter_));
			request->onResponse.connect(boost::bind(&AvatarManager::handleVCardReceived, this, from, _1, _2));
			request->send();
		}
		else {
			onAvatarChanged(from, hash);
		}
	}
}

void AvatarManager::handleVCardReceived(JID from, boost::shared_ptr<VCard> vCard, const boost::optional<Error>& error) {
	if (error) {
		// FIXME: What to do here?
		return;
	}
	String hash = SHA1::getHexHash(vCard->getPhoto());
	avatarStorage_->addAvatar(hash, vCard->getPhoto());
	onAvatarChanged(from, hash);
}

String AvatarManager::getAvatarHash(const JID& jid) const {
	std::map<JID, String>::const_iterator i = avatarHashes_.find(jid.toBare());
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

}
