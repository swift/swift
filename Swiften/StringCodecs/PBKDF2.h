/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PBKDF2 {
		public:
			static ByteArray encode(const ByteArray& password, const ByteArray& salt, int iterations);
	};
}
