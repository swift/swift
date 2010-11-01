/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class WriteBytestream {
		public:
			typedef boost::shared_ptr<WriteBytestream> ref;

			virtual ~WriteBytestream();

			virtual void write(const ByteArray&) = 0;
	};
}
