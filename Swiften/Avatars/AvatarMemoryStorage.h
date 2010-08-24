/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {
	class AvatarMemoryStorage : public AvatarStorage {
		public:
			virtual bool hasAvatar(const String& hash) const { return avatars.find(hash) != avatars.end(); }
			virtual void addAvatar(const String& hash, const ByteArray& avatar) { avatars[hash] = avatar; }
			virtual ByteArray getAvatar(const String& hash) const {
				std::map<String, ByteArray>::const_iterator i = avatars.find(hash);
				return i == avatars.end() ? ByteArray() : i->second;
			}

			virtual boost::filesystem::path getAvatarPath(const String& /*hash*/) const {
				return boost::filesystem::path();
			}

		private:
			std::map<String, ByteArray> avatars;
	};
}
