/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/SecureTransport/SecureTransportCertificate.h>

namespace Swift {

class SecureTransportCertificateFactory : public CertificateFactory {
    public:
        virtual Certificate* createCertificateFromDER(const ByteArray& der) {
            return new SecureTransportCertificate(der);
        }
    };
}
