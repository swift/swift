/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

namespace Swift {
	class String;
	class ByteArray;

	class AvatarStorage {
		public:
			virtual ~AvatarStorage();

			virtual bool hasAvatar(const String& hash) const = 0;
			virtual void addAvatar(const String& hash, const ByteArray& avatar) = 0;
			virtual boost::filesystem::path getAvatarPath(const String& hash) const = 0;
	};

}
