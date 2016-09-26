/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Base/Override.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {
    class WindowsCryptoProvider : public CryptoProvider, public boost::noncopyable {
        public:
            WindowsCryptoProvider();
            virtual ~WindowsCryptoProvider();

            virtual Hash* createSHA1() SWIFTEN_OVERRIDE;
            virtual Hash* createMD5() SWIFTEN_OVERRIDE;
            virtual ByteArray getHMACSHA1(const SafeByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
            virtual ByteArray getHMACSHA1(const ByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
            virtual bool isMD5AllowedForCrypto() const SWIFTEN_OVERRIDE;

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
