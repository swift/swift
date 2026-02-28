/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include <Swift/Controllers/Storages/CertificateStorage.h>

namespace Swift {
    class CertificateFactory;
    class CryptoProvider;

    class CertificateFileStorage : public CertificateStorage {
        public:
            CertificateFileStorage(const boost::filesystem::path& path, CertificateFactory* certificateFactory, CryptoProvider* crypto);

            virtual bool hasCertificate(Certificate::ref certificate) const;
            virtual void addCertificate(Certificate::ref certificate);

        private:
            boost::filesystem::path getCertificatePath(Certificate::ref certificate) const;

        private:
            boost::filesystem::path path;
            CertificateFactory* certificateFactory;
            CryptoProvider* crypto;
    };

}
