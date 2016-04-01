/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
    /**
     * A class to implement a check for certificate trust.
     */
    class SWIFTEN_API CertificateTrustChecker {
        public:
            virtual ~CertificateTrustChecker();

            /**
             * This method is called to find out whether a certificate (chain) is
             * trusted. This usually happens when a certificate's validation
             * fails, to check whether to proceed with the connection or not.
             *
             * certificateChain contains the chain of certificates. The first certificate
             * is the subject certificate.
             */
            virtual bool isCertificateTrusted(const std::vector<Certificate::ref>& certificateChain) = 0;
    };
}
