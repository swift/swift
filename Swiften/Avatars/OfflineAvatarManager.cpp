/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Avatars/OfflineAvatarManager.h>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarStorage.h>

namespace Swift {

OfflineAvatarManager::OfflineAvatarManager(AvatarStorage* avatarStorage) : avatarStorage(avatarStorage) {
}

OfflineAvatarManager::~OfflineAvatarManager() {
}

boost::optional<std::string> OfflineAvatarManager::getAvatarHash(const JID& jid) const {
    return avatarStorage->getAvatarForJID(jid);
}

void OfflineAvatarManager::setAvatar(const JID& jid, const std::string& hash) {
    if (getAvatarHash(jid) != hash) {
        avatarStorage->setAvatarForJID(jid, hash);
        onAvatarChanged(jid);
    }
}

}
