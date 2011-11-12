/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>

#include <Swiften/Network/HTTPConnectProxiedConnection.h>

namespace Swift {

HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(ConnectionFactory* connectionFactory, const HostAddressPort& proxy) : connectionFactory_(connectionFactory), proxy_(proxy), authID_(""), authPassword_("") {
}


HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(ConnectionFactory* connectionFactory, const HostAddressPort& proxy, const SafeString& authID, const SafeString& authPassword) : connectionFactory_(connectionFactory), proxy_(proxy), authID_(authID), authPassword_(authPassword) {
}

boost::shared_ptr<Connection> HTTPConnectProxiedConnectionFactory::createConnection() {
	return HTTPConnectProxiedConnection::create(connectionFactory_, proxy_, authID_, authPassword_);
}

}
