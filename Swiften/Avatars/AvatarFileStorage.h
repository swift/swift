/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/filesystem.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {
	class AvatarFileStorage : public AvatarStorage {
		public:
			AvatarFileStorage(const boost::filesystem::path& path);

			virtual bool hasAvatar(const String& hash) const;
			virtual void addAvatar(const String& hash, const ByteArray& avatar);

			virtual boost::filesystem::path getAvatarPath(const String& hash) const;

		private:
			boost::filesystem::path path_;
	};

}
