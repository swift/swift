/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Crypto/Hash.h>

namespace Swift {
	class Hash;

	class CryptoProvider {
		public:
			virtual ~CryptoProvider();

			virtual Hash* createSHA1() = 0;
			virtual Hash* createMD5() = 0;
			virtual Hash* createHMACSHA1(const SafeByteArray& key) = 0;
			virtual Hash* createHMACSHA1(const ByteArray& key) = 0;
			virtual bool isMD5AllowedForCrypto() const = 0;

			// Convenience
			template<typename T> ByteArray getSHA1Hash(const T& data) {
				return boost::shared_ptr<Hash>(createSHA1())->update(data).getHash();
			}

			template<typename T> ByteArray getMD5Hash(const T& data) {
				return boost::shared_ptr<Hash>(createMD5())->update(data).getHash();
			}

			template<typename T, typename U> ByteArray getHMACSHA1(const T& key, const U& data) {
				return boost::shared_ptr<Hash>(createHMACSHA1(key))->update(data).getHash();
			}
	};
}
