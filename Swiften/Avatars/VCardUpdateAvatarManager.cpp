/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Avatars/VCardUpdateAvatarManager.h>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarStorage.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Elements/VCardUpdate.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/GetVCardRequest.h>
#include <Swiften/VCards/VCardManager.h>

namespace Swift {

VCardUpdateAvatarManager::VCardUpdateAvatarManager(VCardManager* vcardManager, StanzaChannel* stanzaChannel, AvatarStorage* avatarStorage, CryptoProvider* crypto, MUCRegistry* mucRegistry) : vcardManager_(vcardManager), avatarStorage_(avatarStorage), crypto_(crypto), mucRegistry_(mucRegistry) {
    stanzaChannel->onPresenceReceived.connect(boost::bind(&VCardUpdateAvatarManager::handlePresenceReceived, this, _1));
    stanzaChannel->onAvailableChanged.connect(boost::bind(&VCardUpdateAvatarManager::handleStanzaChannelAvailableChanged, this, _1));
    vcardManager_->onVCardChanged.connect(boost::bind(&VCardUpdateAvatarManager::handleVCardChanged, this, _1, _2));
}

void VCardUpdateAvatarManager::handlePresenceReceived(std::shared_ptr<Presence> presence) {
    std::shared_ptr<VCardUpdate> update = presence->getPayload<VCardUpdate>();
    if (!update || presence->getPayload<ErrorPayload>()) {
        return;
    }
    JID from = getAvatarJID(presence->getFrom());
    if (getAvatarHash(from) == update->getPhotoHash()) {
        return;
    }
    SWIFT_LOG(debug) << "Updated hash: " << from << " -> " << update->getPhotoHash() << std::endl;
    if (avatarStorage_->hasAvatar(update->getPhotoHash())) {
        setAvatarHash(from, update->getPhotoHash());
    }
    else {
        vcardManager_->requestVCard(from);
    }
}

void VCardUpdateAvatarManager::handleVCardChanged(const JID& from, VCard::ref vCard) {
    if (!vCard) {
        SWIFT_LOG(debug) << "Missing element: " << from << ": null vcard payload" << std::endl;
        return;
    }

    if (vCard->getPhoto().empty()) {
        setAvatarHash(from, "");
    }
    else {
        std::string hash = Hexify::hexify(crypto_->getSHA1Hash(vCard->getPhoto()));
        if (!avatarStorage_->hasAvatar(hash)) {
            avatarStorage_->addAvatar(hash, vCard->getPhoto());
        }
        setAvatarHash(from, hash);
    }
}

void VCardUpdateAvatarManager::setAvatarHash(const JID& from, const std::string& hash) {
    SWIFT_LOG(debug) << "Updating hash: " << from << " -> " << hash << std::endl;
    avatarHashes_[from] = hash;
    onAvatarChanged(from);
}

/*
void VCardUpdateAvatarManager::setAvatar(const JID& jid, const ByteArray& avatar) {
    std::string hash = Hexify::hexify(SHA1::getHash(avatar));
    avatarStorage_->addAvatar(hash, avatar);
    setAvatarHash(getAvatarJID(jid), hash);
}
*/

boost::optional<std::string> VCardUpdateAvatarManager::getAvatarHash(const JID& jid) const {
    std::map<JID, std::string>::const_iterator i = avatarHashes_.find(getAvatarJID(jid));
    if (i != avatarHashes_.end()) {
        return i->second;
    }
    else {
        return boost::optional<std::string>();
    }
}

JID VCardUpdateAvatarManager::getAvatarJID(const JID& jid) const {
    JID bareFrom = jid.toBare();
    return (mucRegistry_ && mucRegistry_->isMUC(bareFrom)) ? jid : bareFrom;
}

void VCardUpdateAvatarManager::handleStanzaChannelAvailableChanged(bool available) {
    if (available) {
        std::map<JID, std::string> oldAvatarHashes;
        avatarHashes_.swap(oldAvatarHashes);
        for(std::map<JID, std::string>::const_iterator i = oldAvatarHashes.begin(); i != oldAvatarHashes.end(); ++i) {
            onAvatarChanged(i->first);
        }
    }
}


}
