/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/cstdint.hpp>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class SHA1 {
		public:
			SHA1();

			SHA1& update(const std::vector<unsigned char>& data);
			std::vector<unsigned char> getHash() const;

			/**
			 * Equivalent of:
			 *	SHA1().update(data),getHash(), but slightly more efficient and
			 *	convenient.
			 */
			static ByteArray getHash(const ByteArray& data);

		private:
			typedef struct {
					boost::uint32_t state[5];
					boost::uint32_t count[2];
					boost::uint8_t buffer[64];
			} CTX;
			static void Init(CTX* context);
			static void Transform(boost::uint32_t state[5], boost::uint8_t buffer[64]);
			static void Update(CTX* context, boost::uint8_t* data, unsigned int len);
			static void Final(boost::uint8_t digest[20], CTX* context);

		private:
			CTX context;
	};
}
