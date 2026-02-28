/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/TLS/CertificateFactory.h>
#include <Swiften/TLS/Schannel/SchannelCertificate.h>

namespace Swift {
    class SchannelCertificateFactory : public CertificateFactory {
        public:
            virtual Certificate* createCertificateFromDER(const ByteArray& der) {
                return new SchannelCertificate(der);
            }
    };
}
