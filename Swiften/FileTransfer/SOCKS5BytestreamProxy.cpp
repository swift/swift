/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "SOCKS5BytestreamProxy.h"

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/Base/Log.h>

namespace Swift {

SOCKS5BytestreamProxy::SOCKS5BytestreamProxy(ConnectionFactory *connFactory, TimerFactory *timeFactory) : connectionFactory(connFactory), timerFactory(timeFactory) {

}

void SOCKS5BytestreamProxy::addS5BProxy(S5BProxyRequest::ref proxy) {
	localS5BProxies.push_back(proxy);
}

const std::vector<S5BProxyRequest::ref>& SOCKS5BytestreamProxy::getS5BProxies() const {
	return localS5BProxies;
}

void SOCKS5BytestreamProxy::connectToProxies(const std::string& sessionID) {
	SWIFT_LOG(debug) << "session ID: " << sessionID << std::endl;
	ProxyJIDClientSessionMap clientSessions;

	foreach(S5BProxyRequest::ref proxy, localS5BProxies) {
		boost::shared_ptr<Connection> conn = connectionFactory->createConnection();

		boost::shared_ptr<SOCKS5BytestreamClientSession> session = boost::make_shared<SOCKS5BytestreamClientSession>(conn, proxy->getStreamHost().get().addressPort, sessionID, timerFactory);
		clientSessions[proxy->getStreamHost().get().jid] = session;
		session->start();
	}

	proxySessions[sessionID] = clientSessions;
}

boost::shared_ptr<SOCKS5BytestreamClientSession> SOCKS5BytestreamProxy::getProxySessionAndCloseOthers(const JID& proxyJID, const std::string& sessionID) {
	// checking parameters
	if (proxySessions.find(sessionID) == proxySessions.end()) {
		return boost::shared_ptr<SOCKS5BytestreamClientSession>();
	}
	if (proxySessions[sessionID].find(proxyJID) == proxySessions[sessionID].end()) {
		return boost::shared_ptr<SOCKS5BytestreamClientSession>();
	}

	// get active session
	boost::shared_ptr<SOCKS5BytestreamClientSession> activeSession = proxySessions[sessionID][proxyJID];
	proxySessions[sessionID].erase(proxyJID);

	// close other sessions
	foreach(const ProxyJIDClientSessionMap::value_type& myPair, proxySessions[sessionID]) {
		myPair.second->stop();
	}

	proxySessions.erase(sessionID);

	return activeSession;
}

boost::shared_ptr<SOCKS5BytestreamClientSession> SOCKS5BytestreamProxy::createSOCKS5BytestreamClientSession(HostAddressPort addressPort, const std::string& destAddr) {
	SOCKS5BytestreamClientSession::ref connection = boost::make_shared<SOCKS5BytestreamClientSession>(connectionFactory->createConnection(), addressPort, destAddr, timerFactory);
	return connection;
}

}
