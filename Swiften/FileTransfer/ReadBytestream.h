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
	class ReadBytestream {
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
