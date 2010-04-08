/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/UnitTest/MockAvatarManager.h"

namespace Swift {

MockAvatarManager::MockAvatarManager() {

}

MockAvatarManager::~MockAvatarManager() {

}

String MockAvatarManager::getAvatarHash(const JID& jid) const {
	return jid.toBare();
}

boost::filesystem::path MockAvatarManager::getAvatarPath(const JID& jid) const {
	return jid.getResource().getUTF8String();
}

void MockAvatarManager::setAvatar(const JID&, const ByteArray&) {

}

}

