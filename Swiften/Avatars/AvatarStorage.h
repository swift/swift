/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

namespace Swift {
	
	class ByteArray;

	class AvatarStorage {
		public:
			virtual ~AvatarStorage();

			virtual bool hasAvatar(const std::string& hash) const = 0;
			virtual void addAvatar(const std::string& hash, const ByteArray& avatar) = 0;
			virtual ByteArray getAvatar(const std::string& hash) const = 0;
			virtual boost::filesystem::path getAvatarPath(const std::string& hash) const = 0;
	};

}
