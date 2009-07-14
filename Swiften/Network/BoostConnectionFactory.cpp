#include "Swiften/Network/BoostConnectionFactory.h"

namespace Swift {

BoostConnectionFactory::BoostConnectionFactory(boost::asio::io_service* ioService) : ioService(ioService) {
}

BoostConnection* BoostConnectionFactory::createConnection() {
	return new BoostConnection(ioService);
}

}
