/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Storages/StoragesFactory.h"
#include "Swiften/Client/MemoryStorages.h"

namespace Swift {
	class JID;
    class CryptoProvider;
	
	class MemoryStoragesFactory : public StoragesFactory {
		public:
            MemoryStoragesFactory(CryptoProvider* cryptoProvider) : cryptoProvider_(cryptoProvider) {}

			virtual Storages* createStorages(const JID& profile) const {
				return new MemoryStorages(cryptoProvider_);
			}
        private:
            CryptoProvider* cryptoProvider_;
	};
}
