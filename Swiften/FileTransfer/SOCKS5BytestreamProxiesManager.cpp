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

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {

SOCKS5BytestreamProxiesManager::SOCKS5BytestreamProxiesManager(ConnectionFactory *connFactory, TimerFactory *timeFactory, DomainNameResolver* resolver, IQRouter* iqRouter, const JID& serviceRoot) : connectionFactory_(connFactory), timerFactory_(timeFactory), resolver_(resolver), iqRouter_(iqRouter), serviceRoot_(serviceRoot) {

}

SOCKS5BytestreamProxiesManager::~SOCKS5BytestreamProxiesManager() {
	if (proxyFinder_) {
		proxyFinder_->stop();
	}

	foreach (const ProxySessionsMap::value_type& sessionsForID, proxySessions_) {
		foreach (const ProxyJIDClientSessionVector::value_type& session, sessionsForID.second) {
			session.second->onSessionReady.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionReady, this,sessionsForID.first, session.first, session.second, _1));
			session.second->onFinished.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionFinished, this, sessionsForID.first, session.first, session.second, _1));
		}
	}
}

void SOCKS5BytestreamProxiesManager::addS5BProxy(S5BProxyRequest::ref proxy) {
	if (proxy) {
		SWIFT_LOG_ASSERT(HostAddress(proxy->getStreamHost().get().host).isValid(), warning) << std::endl;
		if (!localS5BProxies_) {
			localS5BProxies_ = std::vector<S5BProxyRequest::ref>();
		}
		localS5BProxies_->push_back(proxy);
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
	ProxyJIDClientSessionVector clientSessions;

	if (localS5BProxies_) {
		foreach(S5BProxyRequest::ref proxy, localS5BProxies_.get()) {
			boost::shared_ptr<Connection> conn = connectionFactory_->createConnection();

			HostAddressPort addressPort = HostAddressPort(proxy->getStreamHost().get().host, proxy->getStreamHost().get().port);
			SWIFT_LOG_ASSERT(addressPort.isValid(), warning) << std::endl;
			boost::shared_ptr<SOCKS5BytestreamClientSession> session = boost::make_shared<SOCKS5BytestreamClientSession>(conn, addressPort, sessionID, timerFactory_);
			JID proxyJid = proxy->getStreamHost().get().jid;
			clientSessions.push_back(std::pair<JID, boost::shared_ptr<SOCKS5BytestreamClientSession> >(proxyJid, session));
			session->onSessionReady.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionReady, this,sessionID, proxyJid, session, _1));
			session->onFinished.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionFinished, this, sessionID, proxyJid, session, _1));
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

	// get active session
	boost::shared_ptr<SOCKS5BytestreamClientSession> activeSession;
	for (ProxyJIDClientSessionVector::iterator i = proxySessions_[sessionID].begin(); i != proxySessions_[sessionID].end(); i++) {
		i->second->onSessionReady.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionReady, this,sessionID, proxyJID, i->second, _1));
		i->second->onFinished.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionFinished, this, sessionID, proxyJID, i->second, _1));
		if (i->first == proxyJID && !activeSession) {
			activeSession = i->second;
		}
		else {
			i->second->stop();
		}
	}

	proxySessions_.erase(sessionID);

	return activeSession;
}

boost::shared_ptr<SOCKS5BytestreamClientSession> SOCKS5BytestreamProxiesManager::createSOCKS5BytestreamClientSession(HostAddressPort addressPort, const std::string& destAddr) {
	SOCKS5BytestreamClientSession::ref connection = boost::make_shared<SOCKS5BytestreamClientSession>(connectionFactory_->createConnection(), addressPort, destAddr, timerFactory_);
	return connection;
}

void SOCKS5BytestreamProxiesManager::handleProxiesFound(std::vector<S5BProxyRequest::ref> proxyHosts) {
	foreach(S5BProxyRequest::ref proxy, proxyHosts) {
		if (proxy) {
			if (HostAddress(proxy->getStreamHost().get().host).isValid()) {
				addS5BProxy(proxy);
				onDiscoveredProxiesChanged();
			}
			else {
				DomainNameAddressQuery::ref resolveRequest = resolver_->createAddressQuery(proxy->getStreamHost().get().host);
				resolveRequest->onResult.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleNameLookupResult, this, _1, _2, proxy));
				resolveRequest->run();
			}
		}
	}
	proxyFinder_->stop();
	proxyFinder_.reset();
}

void SOCKS5BytestreamProxiesManager::handleNameLookupResult(const std::vector<HostAddress>& addresses, boost::optional<DomainNameResolveError> error, S5BProxyRequest::ref proxy) {
	if (error) {
		onDiscoveredProxiesChanged();
	}
	else {
		if (addresses.empty()) {
			SWIFT_LOG(warning) << "S5B proxy hostname does not resolve." << std::endl;
			onDiscoveredProxiesChanged();
		}
		else {
			// generate proxy per returned address
			foreach (const HostAddress& address, addresses) {
				S5BProxyRequest::StreamHost streamHost = proxy->getStreamHost().get();
				S5BProxyRequest::ref proxyForAddress = boost::make_shared<S5BProxyRequest>(*proxy);
				streamHost.host = address.toString();
				proxyForAddress->setStreamHost(streamHost);
				addS5BProxy(proxyForAddress);
			}
			onDiscoveredProxiesChanged();
		}
	}
}

void SOCKS5BytestreamProxiesManager::queryForProxies() {
	proxyFinder_ = boost::make_shared<SOCKS5BytestreamProxyFinder>(serviceRoot_, iqRouter_);

	proxyFinder_->onProxiesFound.connect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxiesFound, this, _1));
	proxyFinder_->start();
}

void SOCKS5BytestreamProxiesManager::handleProxySessionReady(const std::string& sessionID, const JID& jid, boost::shared_ptr<SOCKS5BytestreamClientSession> session, bool error) {
	session->onSessionReady.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionReady, this, boost::cref(sessionID), boost::cref(jid), session, _1));
	if (!error) {
		// The SOCKS5 bytestream session to the proxy succeeded; stop and remove other sessions.
		if (proxySessions_.find(sessionID) != proxySessions_.end()) {
			for (ProxyJIDClientSessionVector::iterator i = proxySessions_[sessionID].begin(); i != proxySessions_[sessionID].end();) {
				if ((i->first == jid) && (i->second != session)) {
					i->second->stop();
					i = proxySessions_[sessionID].erase(i);
				}
				else {
					i++;
				}
			}
		}
	}
}

void SOCKS5BytestreamProxiesManager::handleProxySessionFinished(const std::string& sessionID, const JID& jid, boost::shared_ptr<SOCKS5BytestreamClientSession> session, boost::optional<FileTransferError> error) {
	session->onFinished.disconnect(boost::bind(&SOCKS5BytestreamProxiesManager::handleProxySessionFinished, this, sessionID, jid, session, _1));
	if (error.is_initialized()) {
		// The SOCKS5 bytestream session to the proxy failed; remove it.
		if (proxySessions_.find(sessionID) != proxySessions_.end()) {
			for (ProxyJIDClientSessionVector::iterator i = proxySessions_[sessionID].begin(); i != proxySessions_[sessionID].end();) {
				if ((i->first == jid) && (i->second == session)) {
					i->second->stop();
					i = proxySessions_[sessionID].erase(i);
					break;
				}
				else {
					i++;
				}
			}
		}
	}
}

}
