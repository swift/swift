/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateTrustChecker.h>

#include <Swift/Controllers/Storages/CertificateStorage.h>

namespace Swift {
    /**
     * A certificate trust checker that trusts certificates in a certificate storage.
     */
    class CertificateStorageTrustChecker : public CertificateTrustChecker {
        public:
            CertificateStorageTrustChecker(CertificateStorage* storage) : storage(storage) {
            }

            virtual bool isCertificateTrusted(const std::vector<Certificate::ref>& certificateChain) {
                lastCertificateChain = std::vector<Certificate::ref>(certificateChain.begin(), certificateChain.end());
                return certificateChain.empty() ? false : storage->hasCertificate(certificateChain[0]);
            }

            const std::vector<Certificate::ref>& getLastCertificateChain() const {
                return lastCertificateChain;
            }

        private:
            CertificateStorage* storage;
            std::vector<Certificate::ref> lastCertificateChain;
    };
}
