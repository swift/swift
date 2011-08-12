/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Algorithm.h>
#include <cassert>

namespace Swift {
	namespace HMAC_Detail {
		template<typename KeyType> struct KeyWrapper;
		template<> struct KeyWrapper<ByteArray> {
			ByteArray wrap(const ByteArray& hash) const {
				return hash;
			}
		};
		template<> struct KeyWrapper<SafeByteArray> {
			SafeByteArray wrap(const ByteArray& hash) const {
				return createSafeByteArray(hash);
			}
		};

		template<typename Hash, typename KeyType, int BlockSize>
		static ByteArray getHMAC(const KeyType& key, const ByteArray& data) {
			Hash hash;

			// Create the padded key
			KeyType paddedKey(key.size() <= BlockSize ? key : KeyWrapper<KeyType>().wrap(hash(key)));
			paddedKey.resize(BlockSize, 0x0);

			// Create the first value
			KeyType x(paddedKey);
			for (unsigned int i = 0; i < x.size(); ++i) {
				x[i] ^= 0x36;
			}
			append(x, data);

			// Create the second value
			KeyType y(paddedKey);
			for (unsigned int i = 0; i < y.size(); ++i) {
				y[i] ^= 0x5c;
			}
			append(y, hash(x));

			return hash(y);
		}
	};

	template<typename Hash, int BlockSize>
	class HMAC {
		private:

		public:
			ByteArray operator()(const ByteArray& key, const ByteArray& data) const {
				return HMAC_Detail::getHMAC<Hash,ByteArray,BlockSize>(key, data);
			}

			ByteArray operator()(const SafeByteArray& key, const ByteArray& data) const {
				return HMAC_Detail::getHMAC<Hash,SafeByteArray,BlockSize>(key, data);
			}
	};
}
