/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Avatars/VCardAvatarManager.h>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarStorage.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/VCards/VCardManager.h>

namespace Swift {

VCardAvatarManager::VCardAvatarManager(VCardManager* vcardManager, AvatarStorage* avatarStorage, CryptoProvider* crypto, MUCRegistry* mucRegistry) : vcardManager_(vcardManager), avatarStorage_(avatarStorage), crypto_(crypto), mucRegistry_(mucRegistry) {
    vcardManager_->onVCardChanged.connect(boost::bind(&VCardAvatarManager::handleVCardChanged, this, _1));
}

void VCardAvatarManager::handleVCardChanged(const JID& from) {
    // We don't check whether the avatar actually changed. Direct use of this
    // manager could cause unnecessary updates, but in practice, this will be
    // caught by the wrapping CombinedAvatarManager anyway.
    onAvatarChanged(from);
}

boost::optional<std::string> VCardAvatarManager::getAvatarHash(const JID& jid) const {
    JID avatarJID = getAvatarJID(jid);
    std::string hash = vcardManager_->getPhotoHash(avatarJID);
    if (!hash.empty()) {
        if (!avatarStorage_->hasAvatar(hash)) {
            VCard::ref vCard = vcardManager_->getVCard(avatarJID);
            if (vCard) {
                std::string newHash = Hexify::hexify(crypto_->getSHA1Hash(vCard->getPhoto()));
                if (newHash != hash) {
                    // Shouldn't happen, but sometimes seem to. Might be fixed if we
                    // move to a safer backend.
                    SWIFT_LOG(warning) << "Inconsistent vCard photo hash cache";
                    hash = newHash;
                }
                avatarStorage_->addAvatar(hash, vCard->getPhoto());
            }
            else {
                // Can happen if the cache is inconsistent.
                hash = "";
            }
        }
    }
    return hash;
}

JID VCardAvatarManager::getAvatarJID(const JID& jid) const {
    JID bareFrom = jid.toBare();
    return (mucRegistry_ && mucRegistry_->isMUC(bareFrom)) ? jid : bareFrom;
}

}
