/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StringCodecs/PBKDF2.h"
#include "Swiften/StringCodecs/HMACSHA1.h"

namespace Swift {

ByteArray PBKDF2::encode(const ByteArray& password, const ByteArray& salt, int iterations) {
	ByteArray u = HMACSHA1::getResult(password, salt + ByteArray("\0\0\0\1", 4));
	ByteArray result = u;
	int i = 1;
	while (i < iterations) {
		u = HMACSHA1::getResult(password, u);
		for (unsigned int j = 0; j < u.getSize(); ++j) {
			result[j] ^= u[j];
		}
		++i;
	}
	return result;
}

}
