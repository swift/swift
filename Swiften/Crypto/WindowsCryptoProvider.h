/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {
    class WindowsCryptoProvider : public CryptoProvider, public boost::noncopyable {
        public:
            WindowsCryptoProvider();
            virtual ~WindowsCryptoProvider();

            virtual Hash* createSHA1() override;
            virtual Hash* createMD5() override;
            virtual ByteArray getHMACSHA1(const SafeByteArray& key, const ByteArray& data) override;
            virtual ByteArray getHMACSHA1(const ByteArray& key, const ByteArray& data) override;
            virtual bool isMD5AllowedForCrypto() const override;

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
