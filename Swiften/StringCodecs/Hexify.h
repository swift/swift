/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class Hexify {
		public:
			static std::string hexify(unsigned char byte);
			static std::string hexify(const ByteArray& data);
			static ByteArray unhexify(const std::string& hexstring);
	};
}
