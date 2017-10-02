/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Avatars/AvatarManagerImpl.h>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarStorage.h>
#include <Swiften/Avatars/OfflineAvatarManager.h>
#include <Swiften/Avatars/VCardAvatarManager.h>
#include <Swiften/Avatars/VCardUpdateAvatarManager.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

AvatarManagerImpl::AvatarManagerImpl(VCardManager* vcardManager, StanzaChannel* stanzaChannel, AvatarStorage* avatarStorage, CryptoProvider* crypto, MUCRegistry* mucRegistry) : avatarStorage(avatarStorage) {
    vcardUpdateAvatarManager = new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, crypto, mucRegistry);
    combinedAvatarProvider.addProvider(vcardUpdateAvatarManager);

    vcardAvatarManager = new VCardAvatarManager(vcardManager, avatarStorage, crypto, mucRegistry);
    combinedAvatarProvider.addProvider(vcardAvatarManager);

    offlineAvatarManager = new OfflineAvatarManager(avatarStorage);
    combinedAvatarProvider.addProvider(offlineAvatarManager);

    combinedAvatarProvider.onAvatarChanged.connect(boost::bind(&AvatarManagerImpl::handleCombinedAvatarChanged, this, _1));
}

AvatarManagerImpl::~AvatarManagerImpl() {
    combinedAvatarProvider.onAvatarChanged.disconnect(boost::bind(&AvatarManagerImpl::handleCombinedAvatarChanged, this, _1));

    combinedAvatarProvider.removeProvider(offlineAvatarManager);
    delete offlineAvatarManager;
    combinedAvatarProvider.removeProvider(vcardAvatarManager);
    delete vcardAvatarManager;
    combinedAvatarProvider.removeProvider(vcardUpdateAvatarManager);
    delete vcardUpdateAvatarManager;
}

boost::filesystem::path AvatarManagerImpl::getAvatarPath(const JID& jid) const {
    boost::optional<std::string> hash = combinedAvatarProvider.getAvatarHash(jid);
    if (hash && !hash->empty()) {
        return avatarStorage->getAvatarPath(*hash);
    }
    return boost::filesystem::path();
}

ByteArray AvatarManagerImpl::getAvatar(const JID& jid) const {
    boost::optional<std::string> hash = combinedAvatarProvider.getAvatarHash(jid);
    if (hash && !hash->empty()) {
        return avatarStorage->getAvatar(*hash);
    }
    return ByteArray();
}

void AvatarManagerImpl::handleCombinedAvatarChanged(const JID& jid) {
    boost::optional<std::string> hash = combinedAvatarProvider.getAvatarHash(jid);
    assert(hash);
    offlineAvatarManager->setAvatar(jid, *hash);
    onAvatarChanged(jid);
}

}
