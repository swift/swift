/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
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

			~SHA1Hash() {
			}

			virtual Hash& update(const ByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual Hash& update(const SafeByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual std::vector<unsigned char> getHash() {
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

			~MD5Hash() {
			}

			virtual Hash& update(const ByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual Hash& update(const SafeByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual std::vector<unsigned char> getHash() {
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
	class HMACHash : public Hash {
		public:
			HMACHash(const T& key) : finalized(false) {
				HMAC_CTX_init(&context);
				HMAC_Init(&context, vecptr(key), boost::numeric_cast<int>(key.size()), EVP_sha1());
			}

			~HMACHash() {
				HMAC_CTX_cleanup(&context);
			}

			virtual Hash& update(const ByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual Hash& update(const SafeByteArray& data) SWIFTEN_OVERRIDE {
				return updateInternal(data);
			}

			virtual std::vector<unsigned char> getHash() {
				assert(!finalized);
				std::vector<unsigned char> result(SHA_DIGEST_LENGTH);
				unsigned int len = SHA_DIGEST_LENGTH;
				HMAC_Final(&context, vecptr(result), &len);
				return result;
			}

		private:
			template<typename ContainerType>
			Hash& updateInternal(const ContainerType& data) {
				assert(!finalized);
				HMAC_Update(&context, vecptr(data), data.size());
				return *this;
			}

		private:
			HMAC_CTX context;
			bool finalized;
	};
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

Hash* OpenSSLCryptoProvider::createHMACSHA1(const SafeByteArray& key) {
	return new HMACHash<SafeByteArray>(key);
}

Hash* OpenSSLCryptoProvider::createHMACSHA1(const ByteArray& key) {
	return new HMACHash<ByteArray>(key);
}

bool OpenSSLCryptoProvider::isMD5AllowedForCrypto() const {
	return true;
}

