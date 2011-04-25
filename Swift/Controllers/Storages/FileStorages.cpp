/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Storages/FileStorages.h"
#include "Swift/Controllers/Storages/VCardFileStorage.h"
#include "Swift/Controllers/Storages/AvatarFileStorage.h"
#include "Swift/Controllers/Storages/CapsFileStorage.h"
#include "Swift/Controllers/Storages/RosterFileStorage.h"

namespace Swift {

FileStorages::FileStorages(const boost::filesystem::path& baseDir, const JID& jid) {
	std::string profile = jid.toBare();
	vcardStorage = new VCardFileStorage(baseDir / profile / "vcards");
	capsStorage = new CapsFileStorage(baseDir / "caps");
	avatarStorage = new AvatarFileStorage(baseDir / "avatars", baseDir / profile / "avatars");
	rosterStorage = new RosterFileStorage(baseDir / profile / "roster.xml");
}

FileStorages::~FileStorages() {
	delete rosterStorage;
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

RosterStorage* FileStorages::getRosterStorage() const {
	return rosterStorage;
}

}
