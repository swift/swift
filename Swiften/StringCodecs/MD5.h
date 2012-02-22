/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	struct md5_state_s;

	class MD5 {
		public:
			MD5();
			~MD5();

			MD5& update(const std::vector<unsigned char>& data);
			std::vector<unsigned char> getHash();

			static ByteArray getHash(const ByteArray& data);
			static ByteArray getHash(const SafeByteArray& data);
			static bool isAllowedForCrypto();

		private:
			md5_state_s* state;
	};
}
