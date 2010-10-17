/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class ReadBytestream {
		public:
			virtual ~ReadBytestream();
			virtual ByteArray read(size_t size) = 0;
			virtual bool isFinished() const = 0;
	};
}
