/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/filesystem.hpp>

#include <string>
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {
	class AvatarFileStorage : public AvatarStorage {
		public:
			AvatarFileStorage(const boost::filesystem::path& path);

			virtual bool hasAvatar(const std::string& hash) const;
			virtual void addAvatar(const std::string& hash, const ByteArray& avatar);
			virtual ByteArray getAvatar(const std::string& hash) const;

			virtual boost::filesystem::path getAvatarPath(const std::string& hash) const;

		private:
			boost::filesystem::path path_;
	};

}
