/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	typedef std::vector<unsigned char> ByteArray;

	SWIFTEN_API ByteArray createByteArray(const std::string& s);
	SWIFTEN_API ByteArray createByteArray(const char* c);

	inline ByteArray createByteArray(const unsigned char* c, size_t n) {
		return ByteArray(c, c  + n);
	}

	inline ByteArray createByteArray(const char* c, size_t n) {
		return ByteArray(c, c  + n);
	}

	inline ByteArray createByteArray(char c) {
		return std::vector<unsigned char>(1, c);
	}

	template<typename T, typename A>
	static const T* vecptr(const std::vector<T, A>& v) {
		return v.empty() ? NULL : &v[0];
	}

	template<typename T, typename A>
	static T* vecptr(std::vector<T, A>& v) {
		return v.empty() ? NULL : &v[0];
	}
			
	SWIFTEN_API std::string byteArrayToString(const ByteArray& b);

	SWIFTEN_API void readByteArrayFromFile(ByteArray&, const std::string& file);
}

