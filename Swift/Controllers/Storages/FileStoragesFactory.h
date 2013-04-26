/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Storages/StoragesFactory.h"
#include "Swift/Controllers/Storages/FileStorages.h"

namespace Swift {
	class CryptoProvider;

	class FileStoragesFactory : public StoragesFactory {
		public:
			FileStoragesFactory(const boost::filesystem::path& basePath, CryptoProvider* crypto) : basePath(basePath), crypto(crypto) {}

			virtual Storages* createStorages(const JID& profile) const {
				return new FileStorages(basePath, profile, crypto);
			}

		private:
			boost::filesystem::path basePath;
			CryptoProvider* crypto;
	};
}
