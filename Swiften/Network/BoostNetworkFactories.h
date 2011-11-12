/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/BoostIOServiceThread.h>

namespace Swift {
	class EventLoop;
	class NATTraverser;
	class PlatformTLSFactories;

	class BoostNetworkFactories : public NetworkFactories {
		public:
			BoostNetworkFactories(EventLoop* eventLoop);
			~BoostNetworkFactories();

			virtual TimerFactory* getTimerFactory() const {
				return timerFactory;
			}

			virtual ConnectionFactory* getConnectionFactory() const {
				return connectionFactory;
			}

			BoostIOServiceThread* getIOServiceThread() {
				return &ioServiceThread;
			}

			DomainNameResolver* getDomainNameResolver() const {
				return domainNameResolver;
			}

			ConnectionServerFactory* getConnectionServerFactory() const {
				return connectionServerFactory;
			}

			NATTraverser* getNATTraverser() const {
				return natTraverser;
			}

			virtual XMLParserFactory* getXMLParserFactory() const {
				return xmlParserFactory;
			}

			virtual TLSContextFactory* getTLSContextFactory() const;

			virtual ProxyProvider* getProxyProvider() const {
				return proxyProvider;
			}

			virtual EventLoop* getEventLoop() const {
				return eventLoop;
			}

		private:
			BoostIOServiceThread ioServiceThread;
			TimerFactory* timerFactory;
			ConnectionFactory* connectionFactory;
			DomainNameResolver* domainNameResolver;
			ConnectionServerFactory* connectionServerFactory;
			NATTraverser* natTraverser;
			XMLParserFactory* xmlParserFactory;
			PlatformTLSFactories* tlsFactories;
			ProxyProvider* proxyProvider;
			EventLoop* eventLoop;
	};
}
