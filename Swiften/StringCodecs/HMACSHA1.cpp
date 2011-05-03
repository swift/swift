/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StringCodecs/HMACSHA1.h>

#include <cassert>

#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {

static const unsigned int B = 64;

ByteArray HMACSHA1::getResult(const ByteArray& key, const ByteArray& data) {
	assert(key.size() <= B);

	// Create the padded key
	ByteArray paddedKey(key);
	paddedKey.resize(B, 0x0);

	// Create the first value
	ByteArray x(paddedKey);
	for (unsigned int i = 0; i < x.size(); ++i) {
		x[i] ^= 0x36;
	}
	append(x, data);

	// Create the second value
	ByteArray y(paddedKey);
	for (unsigned int i = 0; i < y.size(); ++i) {
		y[i] ^= 0x5c;
	}
	append(y, SHA1::getHash(x));

	return SHA1::getHash(y);
}

#if 0

// A tweaked version of HMACSHA1 that is more than twice as fast as the one above.
// After this, more than 80% is spent in SHA1.
// Optimizations:
// - Avoids using ByteArray/std::vector
// - Uses openssl's SHA1, which is slightly faster
// - Does 'xor' on word basis
// - Passes return value as a parameter

#include <openssl/sha.h>

void HMACSHA1::getResult(const ByteArray& key, const ByteArray& data, ByteArray& result) {
	// Create first value
	size_t xSize = B + data.size();
	unsigned char* x = (unsigned char*) malloc(xSize * sizeof(unsigned char));
	memset(x, 0, B);
	memcpy(x, key.getData(), key.size());
	for (unsigned int i = 0; i < (B>>32); ++i) {
		x[i<<32] ^= 0x36363636;
	}
	memcpy(x + B, data.getData(), data.size());

	// Create the second value
	unsigned char y[B + 20];
	memset(y, 0, B);
	memcpy(y, key.getData(), key.size());
	for (unsigned int i = 0; i < (B>>32); ++i) {
		y[i<<32] ^= 0x5c5c5c5c;
	}
	::SHA1(x, xSize, y + B);
	free(x);

	::SHA1(y, B + 20, (unsigned char*) result.getData());
}

#endif

}
