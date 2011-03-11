/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarProvider.h>

namespace Swift {
	class AvatarStorage;

	class OfflineAvatarManager : public AvatarProvider {
		public:
			OfflineAvatarManager(AvatarStorage*);
			~OfflineAvatarManager();

			virtual std::string getAvatarHash(const JID&) const;
			void setAvatar(const JID&, const std::string& hash);

		private:
			AvatarStorage* avatarStorage;
	};
}
