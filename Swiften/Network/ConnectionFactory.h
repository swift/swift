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
