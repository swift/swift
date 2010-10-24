/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/MemoryStorages.h"
#include "Swiften/VCards/VCardMemoryStorage.h"
#include "Swiften/Avatars/AvatarMemoryStorage.h"
#include "Swiften/Disco/CapsMemoryStorage.h"

namespace Swift {

MemoryStorages::MemoryStorages() {
	vcardStorage = new VCardMemoryStorage();
	capsStorage = new CapsMemoryStorage();
	avatarStorage = new AvatarMemoryStorage();
}

MemoryStorages::~MemoryStorages() {
	delete avatarStorage;
	delete capsStorage;
	delete vcardStorage;
}

VCardStorage* MemoryStorages::getVCardStorage() const {
	return vcardStorage;
}

CapsStorage* MemoryStorages::getCapsStorage() const {
	return capsStorage;
}

AvatarStorage* MemoryStorages::getAvatarStorage() const {
	return avatarStorage;
}

}
