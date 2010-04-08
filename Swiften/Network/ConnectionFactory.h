/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class Connection;

	class ConnectionFactory {
		public:
			virtual ~ConnectionFactory();

			virtual boost::shared_ptr<Connection> createConnection() = 0;
	};
}
