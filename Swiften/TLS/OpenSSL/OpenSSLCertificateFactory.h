/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/OpenSSL/OpenSSLCertificate.h>

namespace Swift {
    class OpenSSLCertificateFactory : public CertificateFactory {
        public:
            virtual Certificate* createCertificateFromDER(const ByteArray& der) {
                return new OpenSSLCertificate(der);
            }
    };
}
