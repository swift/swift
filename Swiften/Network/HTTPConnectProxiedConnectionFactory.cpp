/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>

#include <Swiften/Network/HTTPConnectProxiedConnection.h>

namespace Swift {

HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, std::shared_ptr<HTTPTrafficFilter> httpTrafficFilter) : resolver_(resolver), connectionFactory_(connectionFactory), timerFactory_(timerFactory), proxyHost_(proxyHost), proxyPort_(proxyPort),  authID_(""), authPassword_(""), httpTrafficFilter_(httpTrafficFilter) {
}


HTTPConnectProxiedConnectionFactory::HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, const SafeString& authID, const SafeString& authPassword, std::shared_ptr<HTTPTrafficFilter> httpTrafficFilter) : resolver_(resolver), connectionFactory_(connectionFactory), timerFactory_(timerFactory), proxyHost_(proxyHost), proxyPort_(proxyPort), authID_(authID), authPassword_(authPassword),  httpTrafficFilter_(httpTrafficFilter) {
}

std::shared_ptr<Connection> HTTPConnectProxiedConnectionFactory::createConnection() {
    HTTPConnectProxiedConnection::ref proxyConnection = HTTPConnectProxiedConnection::create(resolver_, connectionFactory_, timerFactory_, proxyHost_, proxyPort_, authID_, authPassword_);
    proxyConnection->setHTTPTrafficFilter(httpTrafficFilter_);
    return proxyConnection;
}

}
