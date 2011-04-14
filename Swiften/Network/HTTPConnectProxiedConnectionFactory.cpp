/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/Network/HTTPConnectProxiedConnectionFactory.h"
#include "Swiften/Network/HTTPConnectProxiedConnection.h"

namespace Swift {

HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(ConnectionFactory* connectionFactory, HostAddressPort proxy)
: connectionFactory_(connectionFactory), proxy_(proxy)
{

}

boost::shared_ptr<Connection> HTTPConnectProxiedConnectionFactory::createConnection() {
	return HTTPConnectProxiedConnection::create(connectionFactory_, proxy_);
}

}
