/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/SOCKS5ProxiedConnectionFactory.h>

#include <Swiften/Network/SOCKS5ProxiedConnection.h>

namespace Swift {

SOCKS5ProxiedConnectionFactory::SOCKS5ProxiedConnectionFactory(ConnectionFactory* connectionFactory, const HostAddressPort& proxy) : connectionFactory_(connectionFactory), proxy_(proxy) {
}

boost::shared_ptr<Connection> SOCKS5ProxiedConnectionFactory::createConnection() {
	return SOCKS5ProxiedConnection::create(connectionFactory_, proxy_);
}

}
