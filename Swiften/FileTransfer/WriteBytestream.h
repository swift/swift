/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class WriteBytestream {
		public:
			typedef boost::shared_ptr<WriteBytestream> ref;

			virtual ~WriteBytestream();

			virtual void write(const std::vector<unsigned char>&) = 0;

			boost::signal<void (const std::vector<unsigned char>&)> onWrite;
	};
}
