/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class CertificateStorage;
    class JID;

    class CertificateStorageFactory {
        public:
            virtual ~CertificateStorageFactory();

            virtual CertificateStorage* createCertificateStorage(const JID& profile) const = 0;
    };
}
