/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Client/MemoryStorages.h>

#include <Swift/Controllers/Storages/StoragesFactory.h>

namespace Swift {
    class JID;
    class CryptoProvider;

    class MemoryStoragesFactory : public StoragesFactory {
        public:
            MemoryStoragesFactory(CryptoProvider* cryptoProvider) : cryptoProvider_(cryptoProvider) {}

            virtual Storages* createStorages(const JID& /*profile*/) const {
                return new MemoryStorages(cryptoProvider_);
            }
        private:
            CryptoProvider* cryptoProvider_;
    };
}
