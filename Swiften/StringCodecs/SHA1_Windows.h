/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <Windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <Wincrypt.h>


#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class SHA1 {
		public:
			SHA1();
			~SHA1();

			SHA1& update(const std::vector<unsigned char>& data);
			std::vector<unsigned char> getHash() const;

			static ByteArray getHash(const ByteArray& data);
			static ByteArray getHash(const SafeByteArray& data);

			ByteArray operator()(const SafeByteArray& data) {
				return getHash(data);
			}

			ByteArray operator()(const ByteArray& data) {
				return getHash(data);
			}

		private:
			SHA1& update(const unsigned char* data, size_t dataSize);

		private:
			HCRYPTPROV hCryptProv;
			HCRYPTHASH hHash;
	};
}
