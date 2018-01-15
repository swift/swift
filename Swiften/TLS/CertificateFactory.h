/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/PrivateKey.h>

namespace Swift {
    class SWIFTEN_API CertificateFactory {
        public:
            virtual ~CertificateFactory();

            virtual Certificate* createCertificateFromDER(const ByteArray& der) = 0;
            virtual std::vector<Certificate::ref> createCertificateChain(const ByteArray& data);
            PrivateKey::ref createPrivateKey(const SafeByteArray& data, boost::optional<SafeByteArray> password = boost::optional<SafeByteArray>());
    };
}
