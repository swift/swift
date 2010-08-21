/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "AvatarStorageFactory.h"
#include "AvatarFileStorage.h"

namespace Swift {
	class AvatarFileStorageFactory: public AvatarStorageFactory {
		public:
			AvatarFileStorageFactory(const boost::filesystem::path& basePath) : basePath(basePath) {
			}

			virtual AvatarStorage* createAvatarStorage(const JID& profile) {
				return new AvatarFileStorage(basePath / profile.getBare().getUTF8String());
			}

		private:
			boost::filesystem::path basePath;
	};
}
