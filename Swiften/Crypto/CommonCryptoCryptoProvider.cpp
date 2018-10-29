/*
 * Copyright (c) 2013-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Crypto/CommonCryptoCryptoProvider.h>

#include <cassert>

#include <boost/numeric/conversion/cast.hpp>

#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonHMAC.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Crypto/Hash.h>

using namespace Swift;

namespace {
    class SHA1Hash : public Hash {
        public:
            SHA1Hash() : finalized(false) {
                if (!CC_SHA1_Init(&context)) {
                    assert(false);
                }
            }

            virtual ~SHA1Hash() override {
            }

            virtual Hash& update(const ByteArray& data) override {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) override {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() override {
                assert(!finalized);
                std::vector<unsigned char> result(CC_SHA1_DIGEST_LENGTH);
                CC_SHA1_Final(vecptr(result), &context);
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                assert(!finalized);
                try {
                    if (!CC_SHA1_Update(&context, vecptr(data), boost::numeric_cast<CC_LONG>(data.size()))) {
                        assert(false);
                    }
                }
                catch (const boost::numeric::bad_numeric_cast&) {
                    assert(false);
                }
                return *this;
            }

        private:
            CC_SHA1_CTX context;
            bool finalized;
    };

    class MD5Hash : public Hash {
        public:
            MD5Hash() : finalized(false) {
                if (!CC_MD5_Init(&context)) {
                    assert(false);
                }
            }

            virtual ~MD5Hash() override {
            }

            virtual Hash& update(const ByteArray& data) override {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) override {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() override {
                assert(!finalized);
                std::vector<unsigned char> result(CC_MD5_DIGEST_LENGTH);
                CC_MD5_Final(vecptr(result), &context);
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                assert(!finalized);
                try {
                    if (!CC_MD5_Update(&context, vecptr(data), boost::numeric_cast<CC_LONG>(data.size()))) {
                        assert(false);
                    }
                }
                catch (const boost::numeric::bad_numeric_cast&) {
                    assert(false);
                }
                return *this;
            }

        private:
            CC_MD5_CTX context;
            bool finalized;
    };

    template<typename T>
    ByteArray getHMACSHA1Internal(const T& key, const ByteArray& data) {
        std::vector<unsigned char> result(CC_SHA1_DIGEST_LENGTH);
        try {
            CCHmac(kCCHmacAlgSHA1, vecptr(key), key.size(), vecptr(data), boost::numeric_cast<CC_LONG>(data.size()), vecptr(result));
        }
        catch (const boost::numeric::bad_numeric_cast&) {
            assert(false);
        }
        return result;
    }
}

CommonCryptoCryptoProvider::CommonCryptoCryptoProvider() {
}

CommonCryptoCryptoProvider::~CommonCryptoCryptoProvider() {
}

Hash* CommonCryptoCryptoProvider::createSHA1() {
    return new SHA1Hash();
}

Hash* CommonCryptoCryptoProvider::createMD5() {
    return new MD5Hash();
}

ByteArray CommonCryptoCryptoProvider::getHMACSHA1(const SafeByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data);
}

ByteArray CommonCryptoCryptoProvider::getHMACSHA1(const ByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data);
}

bool CommonCryptoCryptoProvider::isMD5AllowedForCrypto() const {
    return true;
}

