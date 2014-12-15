/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
	class Connection;

	class SWIFTEN_API ConnectionFactory {
		public:
			virtual ~ConnectionFactory();

			virtual boost::shared_ptr<Connection> createConnection() = 0;
	};
}
