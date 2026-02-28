/*
 * Copyright (c) 2013-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Crypto/OpenSSLCryptoProvider.h>

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <cassert>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Crypto/Hash.h>
#include <Swiften/Base/ByteArray.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace Swift;

namespace {
    class SHA1Hash : public Hash {
        public:
            SHA1Hash() : finalized(false) {
                if (!SHA1_Init(&context)) {
                    assert(false);
                }
            }

            ~SHA1Hash() override {
            }

            virtual Hash& update(const ByteArray& data) override {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) override {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() override {
                assert(!finalized);
                std::vector<unsigned char> result(SHA_DIGEST_LENGTH);
                SHA1_Final(vecptr(result), &context);
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                assert(!finalized);
                if (!SHA1_Update(&context, vecptr(data), data.size())) {
                    assert(false);
                }
                return *this;
            }

        private:
            SHA_CTX context;
            bool finalized;
    };

    class MD5Hash : public Hash {
        public:
            MD5Hash() : finalized(false) {
                if (!MD5_Init(&context)) {
                    assert(false);
                }
            }

            ~MD5Hash() override {
            }

            virtual Hash& update(const ByteArray& data) override {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) override {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() override {
                assert(!finalized);
                std::vector<unsigned char> result(MD5_DIGEST_LENGTH);
                MD5_Final(vecptr(result), &context);
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                assert(!finalized);
                if (!MD5_Update(&context, vecptr(data), data.size())) {
                    assert(false);
                }
                return *this;
            }

        private:
            MD5_CTX context;
            bool finalized;
    };


    template<typename T>
    ByteArray getHMACSHA1Internal(const T& key, const ByteArray& data) {
        unsigned int len = SHA_DIGEST_LENGTH;
        std::vector<unsigned char> result(len);
        try {
            HMAC(EVP_sha1(), vecptr(key), boost::numeric_cast<int>(key.size()), vecptr(data), data.size(), vecptr(result), &len);
        }
        catch (const boost::numeric::bad_numeric_cast&) {
            assert(false);
        }
        return result;
    }
}

OpenSSLCryptoProvider::OpenSSLCryptoProvider() {
}

OpenSSLCryptoProvider::~OpenSSLCryptoProvider() {
}

Hash* OpenSSLCryptoProvider::createSHA1() {
    return new SHA1Hash();
}

Hash* OpenSSLCryptoProvider::createMD5() {
    return new MD5Hash();
}

ByteArray OpenSSLCryptoProvider::getHMACSHA1(const SafeByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data);
}

ByteArray OpenSSLCryptoProvider::getHMACSHA1(const ByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data);
}

bool OpenSSLCryptoProvider::isMD5AllowedForCrypto() const {
    return true;
}

