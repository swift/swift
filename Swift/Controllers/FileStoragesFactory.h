/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/StoragesFactory.h"
#include "Swiften/Client/FileStorages.h"

namespace Swift {
	class FileStoragesFactory : public StoragesFactory {
		public:
			FileStoragesFactory(const boost::filesystem::path& basePath) : basePath(basePath) {}

			virtual Storages* createStorages(const JID& profile) const {
				return new FileStorages(basePath, profile);
			}

		private:
			boost::filesystem::path basePath;
	};
}
