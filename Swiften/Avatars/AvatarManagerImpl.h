/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Avatars/CombinedAvatarProvider.h>

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class StanzaChannel;
	class VCardManager;
	class VCardUpdateAvatarManager;
	class VCardAvatarManager;
	class OfflineAvatarManager;

	class AvatarManagerImpl : public AvatarManager {
		public:
			AvatarManagerImpl(VCardManager*, StanzaChannel*, AvatarStorage*, MUCRegistry* = NULL);
			virtual ~AvatarManagerImpl();

			virtual boost::filesystem::path getAvatarPath(const JID&) const;
			virtual ByteArray getAvatar(const JID&) const;

		private:
			void handleCombinedAvatarChanged(const JID& jid);


		private:
			CombinedAvatarProvider combinedAvatarProvider;
			AvatarStorage* avatarStorage;
			VCardUpdateAvatarManager* vcardUpdateAvatarManager;
			VCardAvatarManager* vcardAvatarManager;
			OfflineAvatarManager* offlineAvatarManager;
	};
}
