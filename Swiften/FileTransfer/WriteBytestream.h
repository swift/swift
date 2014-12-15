/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class SWIFTEN_API WriteBytestream {
		public:
			typedef boost::shared_ptr<WriteBytestream> ref;

			virtual ~WriteBytestream();

			virtual void write(const std::vector<unsigned char>&) = 0;

			boost::signal<void (const std::vector<unsigned char>&)> onWrite;
	};
}
