/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <string>
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class Base64 {
		public:
			static std::string encode(const ByteArray& s);
			static ByteArray decode(const std::string &s);
	};
}
