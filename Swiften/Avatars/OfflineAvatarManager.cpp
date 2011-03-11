/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Avatars/OfflineAvatarManager.h>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarStorage.h>

namespace Swift {

OfflineAvatarManager::OfflineAvatarManager(AvatarStorage* avatarStorage) : avatarStorage(avatarStorage) {
}

OfflineAvatarManager::~OfflineAvatarManager() {
}

std::string OfflineAvatarManager::getAvatarHash(const JID& jid) const {
	return avatarStorage->getAvatarForJID(jid);
}

void OfflineAvatarManager::setAvatar(const JID& jid, const std::string& hash) {
	if (getAvatarHash(jid) != hash) {
		avatarStorage->setAvatarForJID(jid, hash);
		onAvatarChanged(jid);
	}
}

}
