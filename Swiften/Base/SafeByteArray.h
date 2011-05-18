/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/SafeAllocator.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class SafeString;

	typedef std::vector<unsigned char, SafeAllocator<unsigned char> > SafeByteArray;

	inline SafeByteArray createSafeByteArray(const ByteArray& a) {
		return SafeByteArray(a.begin(), a.end());
	}

	SafeByteArray createSafeByteArray(const char* c);

	inline SafeByteArray createSafeByteArray(const std::string& s) {
		return SafeByteArray(s.begin(), s.end());
	}

	inline SafeByteArray createSafeByteArray(char c) {
		return SafeByteArray(1, c);
	}

	inline SafeByteArray createSafeByteArray(const char* c, size_t n) {
		return SafeByteArray(c, c + n);
	}

	SafeByteArray createSafeByteArray(const SafeString& s);
}

