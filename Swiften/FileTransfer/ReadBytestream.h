/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class SWIFTEN_API ReadBytestream {
		public:
			virtual ~ReadBytestream();

			/**
			 * Return an empty vector if no more data is available.
			 * Use onDataAvailable signal for signaling there is data available again.
			 */
			virtual boost::shared_ptr< std::vector<unsigned char> > read(size_t size) = 0;

			virtual bool isFinished() const = 0;

		public:
			boost::signal<void ()> onDataAvailable;
			boost::signal<void (const std::vector<unsigned char>&)> onRead;
	};
}
