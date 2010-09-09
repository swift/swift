/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/AvatarManager.h"

#include <boost/bind.hpp>

#include "Swiften/Avatars/VCardUpdateAvatarManager.h"
#include "Swiften/Avatars/VCardAvatarManager.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {

AvatarManager::AvatarManager(VCardManager* vcardManager, StanzaChannel* stanzaChannel, AvatarStorage* avatarStorage, MUCRegistry* mucRegistry) : avatarStorage(avatarStorage) {
	vcardUpdateAvatarManager = new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, mucRegistry);
	combinedAvatarProvider.addProvider(vcardUpdateAvatarManager);

	vcardAvatarManager = new VCardAvatarManager(vcardManager, avatarStorage, mucRegistry);
	combinedAvatarProvider.addProvider(vcardAvatarManager);

	combinedAvatarProvider.onAvatarChanged.connect(boost::ref(onAvatarChanged));
}

AvatarManager::~AvatarManager() {
	combinedAvatarProvider.removeProvider(vcardAvatarManager);
	delete vcardAvatarManager;
	combinedAvatarProvider.removeProvider(vcardUpdateAvatarManager);
	delete vcardUpdateAvatarManager;
}

boost::filesystem::path AvatarManager::getAvatarPath(const JID& jid) const {
	String hash = combinedAvatarProvider.getAvatarHash(jid);
	if (!hash.isEmpty()) {
		std::cout << "getAvatar " << jid << " " << avatarStorage->getAvatarPath(hash) << std::endl;
		return avatarStorage->getAvatarPath(hash);
	}
	return boost::filesystem::path();
}


}
