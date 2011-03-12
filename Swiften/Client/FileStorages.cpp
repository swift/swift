/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/FileStorages.h"
#include "Swiften/VCards/VCardFileStorage.h"
#include "Swiften/Avatars/AvatarFileStorage.h"
#include "Swiften/Disco/CapsFileStorage.h"

namespace Swift {

FileStorages::FileStorages(const boost::filesystem::path& baseDir, const JID& jid) {
	std::string profile = jid.toBare();
	vcardStorage = new VCardFileStorage(baseDir / profile / "vcards");
	capsStorage = new CapsFileStorage(baseDir / "caps");
	avatarStorage = new AvatarFileStorage(baseDir / "avatars", baseDir / profile / "avatars");
}

FileStorages::~FileStorages() {
	delete avatarStorage;
	delete capsStorage;
	delete vcardStorage;
}

VCardStorage* FileStorages::getVCardStorage() const {
	return vcardStorage;
}

CapsStorage* FileStorages::getCapsStorage() const {
	return capsStorage;
}

AvatarStorage* FileStorages::getAvatarStorage() const {
	return avatarStorage;
}

}
