/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Storages/CertificateMemoryStorage.h>
#include <Swift/Controllers/Storages/CertificateStorageFactory.h>

namespace Swift {
    class CertificateFactory;

    class CertificateMemoryStorageFactory : public CertificateStorageFactory {
        public:
            CertificateMemoryStorageFactory() {
            }

            virtual CertificateStorage* createCertificateStorage(const JID&) const {
                return new CertificateMemoryStorage();
            }

        private:
            boost::filesystem::path basePath;
            CertificateFactory* certificateFactory;
    };
}
