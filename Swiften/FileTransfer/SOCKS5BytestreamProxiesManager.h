/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {
	class SOCKS5BytestreamProxiesDiscoverRequest;

	/**
	 *	- manages list of working S5B proxies
	 *	- creates initial connections (for the candidates you provide)
	 */
	class SWIFTEN_API SOCKS5BytestreamProxiesManager {
		public:
			SOCKS5BytestreamProxiesManager(ConnectionFactory*, TimerFactory*);

			boost::shared_ptr<SOCKS5BytestreamProxiesDiscoverRequest> createDiscoverProxiesRequest();

			void addS5BProxy(S5BProxyRequest::ref);
			const std::vector<S5BProxyRequest::ref>& getS5BProxies() const;

			void connectToProxies(const std::string& sessionID);
			boost::shared_ptr<SOCKS5BytestreamClientSession> getProxySessionAndCloseOthers(const JID& proxyJID, const std::string& sessionID);

			boost::shared_ptr<SOCKS5BytestreamClientSession> createSOCKS5BytestreamClientSession(HostAddressPort addressPort, const std::string& destAddr);

		private:
			ConnectionFactory* connectionFactory;
			TimerFactory* timerFactory;

			typedef std::map<JID, boost::shared_ptr<SOCKS5BytestreamClientSession> > ProxyJIDClientSessionMap;
			std::map<std::string, ProxyJIDClientSessionMap> proxySessions;

			std::vector<S5BProxyRequest::ref> localS5BProxies;
	};

}
