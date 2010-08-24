/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/AvatarManager.h"

#include <boost/bind.hpp>

#include "Swiften/Avatars/VCardUpdateAvatarManager.h"

namespace Swift {

AvatarManager::AvatarManager(VCardManager* vcardManager, StanzaChannel* stanzaChannel, AvatarStorage* avatarStorage, MUCRegistry* mucRegistry) {
	vcardUpdateAvatarManager = new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, mucRegistry);
	vcardUpdateAvatarManager->onAvatarChanged.connect(boost::ref(onAvatarChanged));
}

AvatarManager::~AvatarManager() {
	delete vcardUpdateAvatarManager;
}

void AvatarManager::setMUCRegistry(MUCRegistry* mucRegistry) {
	vcardUpdateAvatarManager->setMUCRegistry(mucRegistry);
}

boost::filesystem::path AvatarManager::getAvatarPath(const JID& jid) const {
	return vcardUpdateAvatarManager->getAvatarPath(jid);
}


}
