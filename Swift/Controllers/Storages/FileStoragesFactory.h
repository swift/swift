/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
