/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

namespace Swift {
	class ReadBytestream {
		public:
			virtual ~ReadBytestream();
			virtual std::vector<unsigned char> read(size_t size) = 0;
			virtual bool isFinished() const = 0;
	};
}
