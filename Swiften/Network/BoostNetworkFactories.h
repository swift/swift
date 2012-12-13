/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/BoostIOServiceThread.h>

namespace Swift {
	class EventLoop;
	class NATTraverser;
	class PlatformTLSFactories;

	class SWIFTEN_API BoostNetworkFactories : public NetworkFactories {
		public:
			BoostNetworkFactories(EventLoop* eventLoop);
			~BoostNetworkFactories();

			virtual TimerFactory* getTimerFactory() const SWIFTEN_OVERRIDE {
				return timerFactory;
			}

			virtual ConnectionFactory* getConnectionFactory() const SWIFTEN_OVERRIDE {
				return connectionFactory;
			}

			BoostIOServiceThread* getIOServiceThread() {
				return &ioServiceThread;
			}

			DomainNameResolver* getDomainNameResolver() const SWIFTEN_OVERRIDE {
				return domainNameResolver;
			}

			ConnectionServerFactory* getConnectionServerFactory() const SWIFTEN_OVERRIDE {
				return connectionServerFactory;
			}

			NATTraverser* getNATTraverser() const SWIFTEN_OVERRIDE {
				return natTraverser;
			}

			virtual XMLParserFactory* getXMLParserFactory() const SWIFTEN_OVERRIDE {
				return xmlParserFactory;
			}

			virtual TLSContextFactory* getTLSContextFactory() const SWIFTEN_OVERRIDE;

			virtual ProxyProvider* getProxyProvider() const SWIFTEN_OVERRIDE {
				return proxyProvider;
			}

			virtual EventLoop* getEventLoop() const SWIFTEN_OVERRIDE {
				return eventLoop;
			}

			virtual IDNConverter* getIDNConverter() const SWIFTEN_OVERRIDE {
				return idnConverter;
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
			IDNConverter* idnConverter;
	};
}
