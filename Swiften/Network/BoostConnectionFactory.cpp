/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/BoostConnectionFactory.h"
#include "Swiften/Network/BoostConnection.h"

namespace Swift {

BoostConnectionFactory::BoostConnectionFactory(boost::asio::io_service* ioService) : ioService(ioService) {
}

boost::shared_ptr<Connection> BoostConnectionFactory::createConnection() {
	return boost::shared_ptr<Connection>(new BoostConnection(ioService));
}

}
