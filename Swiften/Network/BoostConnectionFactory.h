#ifndef SWIFTEN_BoostConnectionFactory_H
#define SWIFTEN_BoostConnectionFactory_H

#include <boost/asio.hpp>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/BoostConnection.h"

namespace Swift {
	class BoostConnection;

	class BoostConnectionFactory : public ConnectionFactory {
		public:
			BoostConnectionFactory(boost::asio::io_service*);

			virtual BoostConnection* createConnection();

		private:
			boost::asio::io_service* ioService;
	};
}

#endif
