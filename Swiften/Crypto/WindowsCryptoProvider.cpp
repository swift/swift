/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Crypto/WindowsCryptoProvider.h>

#include <Windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <Wincrypt.h>
#include <cassert>
#include <memory>

#include <Swiften/Crypto/Hash.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/WindowsRegistry.h>

using namespace Swift;

struct WindowsCryptoProvider::Private {
    HCRYPTPROV context;
};

namespace {
    class WindowsHash : public Hash {
        public:
            WindowsHash(HCRYPTPROV context, ALG_ID algorithm) : hash(NULL) {
                if (!CryptCreateHash(context, algorithm, 0, 0, &hash)) {
                    assert(false);
                }
            }

            virtual ~WindowsHash() {
                CryptDestroyHash(hash);
            }

            virtual Hash& update(const ByteArray& data) SWIFTEN_OVERRIDE {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) SWIFTEN_OVERRIDE {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() {
                std::vector<unsigned char> result;
                DWORD hashLength = sizeof(DWORD);
                DWORD hashSize;
                CryptGetHashParam(hash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), &hashLength, 0);
                result.resize(static_cast<size_t>(hashSize));
                if (!CryptGetHashParam(hash, HP_HASHVAL, vecptr(result), &hashSize, 0)) {
                    assert(false);
                }
                result.resize(static_cast<size_t>(hashSize));
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                if (!CryptHashData(hash, const_cast<BYTE*>(vecptr(data)), data.size(), 0)) {
                    assert(false);
                }
                return *this;
            }

        private:
            HCRYPTHASH hash;
    };

#if 0 // NOT YET DONE
    // Haven't tested the code below properly yet, but figured out after writing
    // it that PLAINTEXTKEYBLOB doesn't work on XP or 2k, and the workaround is a
    // bit too ugly to try this now. So, using our own algorithm for now.  See
    // http://support.microsoft.com/kb/228786/en-us

    // MSDN describes this as PLAINTEXTKEYBLOB, but this struct doesn't exist,
    // and seems to even conflict with the PLAINTEXTKEYBLOB constant. Redefining
    // here.
    struct PlainTextKeyBlob {
        BLOBHEADER hdr;
        DWORD dwKeySize;
    };

    class HMACHash : public Hash {
        public:
            template<typename T>
            HMACHash(HCRYPTPROV context, const T& rawKey) : hash(NULL) {
                // Import raw key
                T blobData(sizeof(PlainTextKeyBlob) + rawKey.size());
                PlainTextKeyBlob* blob = reinterpret_cast<PlainTextKeyBlob*>(vecptr(blobData));
                blob->hdr.bType = PLAINTEXTKEYBLOB;
                blob->hdr.bVersion = CUR_BLOB_VERSION;
                blob->hdr.reserved = 0;
                blob->hdr.aiKeyAlg = CALG_RC2;
                blob->dwKeySize = rawKey.size();
                std::copy(rawKey.begin(), rawKey.end(), blobData.begin() + sizeof(PlainTextKeyBlob));
                HCRYPTKEY key;
                if (!CryptImportKey(context, vecptr(blobData), blobData.size(), 0, CRYPT_IPSEC_HMAC_KEY, &key)) {
                    assert(false);
                    return;
                }

                // Create hash
                if (!CryptCreateHash(context, CALG_HMAC, key, 0, &hash)) {
                    assert(false);
                    return;
                }
                ZeroMemory(&info, sizeof(info));
                info.HashAlgid = CALG_SHA1;
            }

            ~HMACHash() {
                CryptDestroyHash(hash);
            }

            virtual Hash& update(const ByteArray& data) SWIFTEN_OVERRIDE {
                return updateInternal(data);
            }

            virtual Hash& update(const SafeByteArray& data) SWIFTEN_OVERRIDE {
                return updateInternal(data);
            }

            virtual std::vector<unsigned char> getHash() {
                std::vector<unsigned char> result;
                DWORD hashLength = sizeof(DWORD);
                DWORD hashSize;
                CryptGetHashParam(hash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), &hashLength, 0);
                result.resize(static_cast<size_t>(hashSize));
                if (!CryptGetHashParam(hash, HP_HASHVAL, vecptr(result), &hashSize, 0)) {
                    assert(false);
                }
                result.resize(static_cast<size_t>(hashSize));
                return result;
            }

        private:
            template<typename ContainerType>
            Hash& updateInternal(const ContainerType& data) {
                if (!CryptHashData(hash, const_cast<BYTE*>(vecptr(data)), data.size(), 0)) {
                    assert(false);
                }
                return *this;
            }

        private:
            HCRYPTHASH hash;
            HMAC_INFO info;
    };
#endif

    // Simple implementation.
    template<typename T>
    ByteArray getHMACSHA1Internal(const T& key, const ByteArray& data, CryptoProvider* crypto) {
        static const int BLOCK_SIZE = 64;

        T paddedKey;
        if (key.size() <= BLOCK_SIZE) {
            paddedKey = key;
        }
        else {
            assign(paddedKey, crypto->getSHA1Hash(key));
        }
        paddedKey.resize(BLOCK_SIZE, 0x0);

        // Create the first value
        T x(paddedKey);
        for (unsigned int i = 0; i < x.size(); ++i) {
            x[i] ^= 0x36;
        }
        append(x, data);

        // Create the second value
        T y(paddedKey);
        for (unsigned int i = 0; i < y.size(); ++i) {
            y[i] ^= 0x5c;
        }
        append(y, crypto->getSHA1Hash(x));
        return crypto->getSHA1Hash(y);
    }
}

WindowsCryptoProvider::WindowsCryptoProvider() : p(new Private()){
    if (!CryptAcquireContext(&p->context, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        assert(false);
    }
}

WindowsCryptoProvider::~WindowsCryptoProvider() {
    CryptReleaseContext(p->context, 0);
}

Hash* WindowsCryptoProvider::createSHA1() {
    return new WindowsHash(p->context, CALG_SHA1);
}

Hash* WindowsCryptoProvider::createMD5() {
    return new WindowsHash(p->context, CALG_MD5);
}

bool WindowsCryptoProvider::isMD5AllowedForCrypto() const {
    return !WindowsRegistry::isFIPSEnabled();
}

ByteArray WindowsCryptoProvider::getHMACSHA1(const SafeByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data, this);
}

ByteArray WindowsCryptoProvider::getHMACSHA1(const ByteArray& key, const ByteArray& data) {
    return getHMACSHA1Internal(key, data, this);
}
