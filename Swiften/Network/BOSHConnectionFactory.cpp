/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "BOSHConnectionFactory.h"
#include <Swiften/Network/BOSHConnection.h>

namespace Swift {

BOSHConnectionFactory::BOSHConnectionFactory(ConnectionFactory* connectionFactory) {
	connectionFactory_ = connectionFactory;

}

boost::shared_ptr<Connection> BOSHConnectionFactory::createConnection() {
	return BOSHConnection::create(connectionFactory_);
}

}
