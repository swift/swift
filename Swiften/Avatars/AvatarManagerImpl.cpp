/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Avatars/AvatarManagerImpl.h"

#include <boost/bind.hpp>

#include "Swiften/Avatars/VCardUpdateAvatarManager.h"
#include "Swiften/Avatars/VCardAvatarManager.h"
#include "Swiften/Avatars/AvatarStorage.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {

AvatarManagerImpl::AvatarManagerImpl(VCardManager* vcardManager, StanzaChannel* stanzaChannel, AvatarStorage* avatarStorage, MUCRegistry* mucRegistry) : avatarStorage(avatarStorage) {
	vcardUpdateAvatarManager = new VCardUpdateAvatarManager(vcardManager, stanzaChannel, avatarStorage, mucRegistry);
	combinedAvatarProvider.addProvider(vcardUpdateAvatarManager);

	vcardAvatarManager = new VCardAvatarManager(vcardManager, avatarStorage, mucRegistry);
	combinedAvatarProvider.addProvider(vcardAvatarManager);

	combinedAvatarProvider.onAvatarChanged.connect(boost::ref(onAvatarChanged));
}

AvatarManagerImpl::~AvatarManagerImpl() {
	combinedAvatarProvider.removeProvider(vcardAvatarManager);
	delete vcardAvatarManager;
	combinedAvatarProvider.removeProvider(vcardUpdateAvatarManager);
	delete vcardUpdateAvatarManager;
}

boost::filesystem::path AvatarManagerImpl::getAvatarPath(const JID& jid) const {
	std::string hash = combinedAvatarProvider.getAvatarHash(jid);
	if (!hash.empty()) {
		return avatarStorage->getAvatarPath(hash);
	}
	return boost::filesystem::path();
}

ByteArray AvatarManagerImpl::getAvatar(const JID& jid) const {
	std::string hash = combinedAvatarProvider.getAvatarHash(jid);
	if (!hash.empty()) {
		return avatarStorage->getAvatar(hash);
	}
	return ByteArray();
}

}
