/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class VCardManager;

	class VCardAvatarManager : public AvatarProvider {
		public:
			VCardAvatarManager(VCardManager*, AvatarStorage*, MUCRegistry* = NULL);

			std::string getAvatarHash(const JID&) const;

		private:
			void handleVCardChanged(const JID& from);
			JID getAvatarJID(const JID& o) const;

		private:
			VCardManager* vcardManager_;
			AvatarStorage* avatarStorage_;
			MUCRegistry* mucRegistry_;
	};
}
