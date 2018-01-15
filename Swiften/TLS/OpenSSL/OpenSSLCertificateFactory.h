/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/OpenSSL/OpenSSLCertificate.h>

namespace Swift {
    class OpenSSLCertificateFactory : public CertificateFactory {
        public:
            OpenSSLCertificateFactory();
            virtual ~OpenSSLCertificateFactory() override final;

            virtual Certificate* createCertificateFromDER(const ByteArray& der) override final;
            virtual std::vector<Certificate::ref> createCertificateChain(const ByteArray& data) override final;
    };
}
