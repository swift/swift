/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DomainNameResolveError.h>

namespace Swift {

SOCKS5BytestreamProxiesManager::SOCKS5BytestreamProxiesManager(ConnectionFactory *connFactory, TimerFactory *timeFactory, DomainNameResolver* resolver, IQRouter* iqRouter, const JID& serviceRoot) : connectionFactory_(connFactory), timerFactory_(timeFactory), resolver_(resolver), iqRouter_(iqRouter), serviceRoot_(serviceRoot) {

}

SOCKS5BytestreamProxiesManager::~SOCKS5BytestreamProxiesManager() {
	if (proxyFinder_) {
		proxyFinder_->stop();
	}
}

void SOCKS5BytestreamProxiesManager::addS5BProxy(S5BProxyRequest::ref proxy) {
	if (proxy) {
		SWIFT_LOG_ASSERT(HostAddress(proxy->getStreamHost().get().host).isValid(), warning) << std::endl;
		if (!localS5BProxies_) {
			localS5BProxies_ = std::vector<S5BProxyRequest::ref>();
		}
		localS5BProxies_->push_back(proxy);
		onDiscoveredProxiesChanged();
	}
}

const boost::optional<std::vector<S5BProxyRequest::ref> >& SOCKS5BytestreamProxiesManager::getOrDiscoverS5BProxies() {
	if (!localS5BProxies_ && !proxyFinder_) {
		queryForProxies();
	}
	return localS5BProxies_;
}

void SOCKS5BytestreamProxiesManager::connectToProxies(const std::string& sessionID) {
	SWIFT_LOG(debug) << "session ID: " << sessionID << std::endl;
	ProxyJIDClientSessionMap clientSessions;

	if (localS5BProxies_) {
		foreach(S5BProxyRequest::ref proxy, localS5BProxies_.get()) {
			boost::shared_ptr<Connection> conn = connectionFactory_->createConnection();

			HostAddressPort addressPort = HostAddressPort(proxy->getStreamHost().get().host, proxy->getStreamHost().get().port);
			SWIFT_LOG_ASSERT(addressPort.isValid(), warning) << std::endl;
			boost::shared_ptr<SOCKS5BytestreamClientSession> session = boost::make_shared<SOCKS5BytestreamClientSession>(conn, addressPort, sessionID, timerFactory_);
			clientSessions[proxy->getStreamHost().get().jid] = session;
			session->start();
		}
	}

	proxySessions_[sessionID] = clientSessions;
}

boost::shared_ptr<SOCKS5BytestreamClientSession> SOCKS5BytestreamProxiesManager::getProxySessionAndCloseOthers(const JID& proxyJID, const std::string& sessionID) {
	// checking parameters
	if (proxySessions_.find(sessionID) == proxySessions_.end()) {
		return boost::shared_ptr<SOCKS5BytestreamClientSession>();
	}
	if (proxySessions_[sessionID].find(proxyJID) == proxySessions_[sessionID].end()) {
		return boost::shared_ptr<SOCKS5BytestreamClientSession>();
	}

	// get active session
	boost::shared_ptr<SOCKS5BytestreamClientSession> activeSession = proxySessions_[sessionID][proxyJID];
	proxySessions_[sessionID].erase(proxyJID);

	// close other sessions
	foreach(const ProxyJIDClientSessionMap::value_type& myPair, proxySessions_[sessionID]) {
		myPair.second->stop();
	}

	proxySessions_.erase(sessionID);

	return activeSession;
}

boost::shared_ptr<SOCKS5BytestreamClientSession> SOCKS5BytestreamProxiesManager::createSOCKS5BytestreamClientSession(HostAddressPort addressPort, const std::string& destAddr) {
	SOCKS5BytestreamClientSession::ref connection = boost::make_shared<SOCKS5BytestreamClientSession>(connectionFactory_->createConnection(), addressPort, destAddr, timerFactory_);
	return connection;
}

void SOCKS5BytestreamProxiesManager::handleProxyFound(S5BProxyRequest::ref proxy) {
	if (proxy) {
		if (HostAddress(proxy->getStreamHost().get().host).isValid()) {
			addS5BProxy(proxy);
		}
		else {
			DomainNameAddressQuery::ref resolveRequest = resolver_->createAddressQuery(proxy->getStreamHost().get().host);
			resolveRequest->onResult.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleNameLookupResult, this, _1, _2, proxy));
			resolveRequest->run();
		}
	}
	else {
		onDiscoveredProxiesChanged();
	}
	proxyFinder_->stop();
	proxyFinder_.reset();
}

void SOCKS5BytestreamProxiesManager::handleNameLookupResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error, S5BProxyRequest::ref proxy) {
	if (error) {
		onDiscoveredProxiesChanged();
	}
	else {
		if (address.empty()) {
			SWIFT_LOG(warning) << "S5B proxy hostname does not resolve." << std::endl;
			onDiscoveredProxiesChanged();
		}
		else {
			S5BProxyRequest::StreamHost streamHost = proxy->getStreamHost().get();
			streamHost.host = address[0].toString();
			proxy->setStreamHost(streamHost);
			addS5BProxy(proxy);
		}
	}
}

void SOCKS5BytestreamProxiesManager::queryForProxies() {
	proxyFinder_ = boost::make_shared<SOCKS5BytestreamProxyFinder>(serviceRoot_, iqRouter_);

	proxyFinder_->onProxyFound.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxyFound, this, _1));
	proxyFinder_->start();
}

}
