/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <map>

#include "Swiften/Avatars/AvatarProvider.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class CombinedAvatarProvider : public AvatarProvider {
		public:
			virtual String getAvatarHash(const JID&) const;

			void addProvider(AvatarProvider*);
			void removeProvider(AvatarProvider*);

		private:
			void handleAvatarChanged(const JID&);

		private:
			std::vector<AvatarProvider*> providers;
			std::map<JID, String> avatars;
	};
}
