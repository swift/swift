/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/SafeByteArray.h>

using namespace Swift;

namespace Swift {

SafeByteArray createSafeByteArray(const char* c) {
	SafeByteArray data;
	while (*c) {
		data.push_back(static_cast<unsigned char>(*c));
		++c;
	}
	return data;
}

}
