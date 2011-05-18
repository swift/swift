/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class MD5 {
		public:
			static ByteArray getHash(const ByteArray& data);
			static ByteArray getHash(const SafeByteArray& data);
	};
}
