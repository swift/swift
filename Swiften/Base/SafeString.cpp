/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/SafeString.h>

#include <algorithm>

using namespace Swift;

SafeString::SafeString(const char* rawData) {
	for (const char* c = rawData; *c; ++c) {
		data.push_back(*c);
	}
}
