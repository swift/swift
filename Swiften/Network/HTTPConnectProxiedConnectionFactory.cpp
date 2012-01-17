/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>

#include <Swiften/Network/HTTPConnectProxiedConnection.h>

namespace Swift {

HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort) : resolver_(resolver), connectionFactory_(connectionFactory), timerFactory_(timerFactory), eventLoop_(eventLoop), proxyHost_(proxyHost), proxyPort_(proxyPort),  authID_(""), authPassword_("") {
}


HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword) : resolver_(resolver), connectionFactory_(connectionFactory), timerFactory_(timerFactory), eventLoop_(eventLoop), proxyHost_(proxyHost), proxyPort_(proxyPort), authID_(authID), authPassword_(authPassword) {
}

boost::shared_ptr<Connection> HTTPConnectProxiedConnectionFactory::createConnection() {
	return HTTPConnectProxiedConnection::create(resolver_, connectionFactory_, timerFactory_, eventLoop_, proxyHost_, proxyPort_, authID_, authPassword_);
}

}
