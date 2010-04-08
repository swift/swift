/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StringCodecs/HMACSHA1.h"

#include <cassert>

#include "Swiften/StringCodecs/SHA1.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {

static const unsigned int B = 64;

ByteArray HMACSHA1::getResult(const ByteArray& key, const ByteArray& data) {
	assert(key.getSize() <= B);

	// Create the padded key
	ByteArray paddedKey(key);
	paddedKey.resize(B);
	for (unsigned int i = key.getSize(); i < paddedKey.getSize(); ++i) {
		paddedKey[i] = 0x0;
	}

	// Create the first value
	ByteArray x(paddedKey);
	for (unsigned int i = 0; i < x.getSize(); ++i) {
		x[i] ^= 0x36;
	}
	x += data;

	// Create the second value
	ByteArray y(paddedKey);
	for (unsigned int i = 0; i < y.getSize(); ++i) {
		y[i] ^= 0x5c;
	}
	y += SHA1::getHash(x);

	return SHA1::getHash(y);
}

}
