#pragma once

#include <boost/asio.hpp>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/BoostConnection.h"

namespace Swift {
	class BoostConnection;

	class BoostConnectionFactory : public ConnectionFactory {
		public:
			BoostConnectionFactory(boost::asio::io_service*);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			boost::asio::io_service* ioService;
	};
}
