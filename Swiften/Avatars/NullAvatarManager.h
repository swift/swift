/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Avatars/AvatarManager.h>

namespace Swift {
	class NullAvatarManager : public AvatarManager {
		public:
			virtual boost::filesystem::path getAvatarPath(const JID&) const {
				return boost::filesystem::path();
			}

			virtual ByteArray getAvatar(const JID&) const {
				return ByteArray();
			}
	};
}
