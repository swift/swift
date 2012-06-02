/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ProxiedConnection.h>

namespace Swift {
	class ConnectionFactory;
	class DomainNameResolver;
	class TimerFactory;

	class SOCKS5ProxiedConnection : public ProxiedConnection {
		public:
			typedef boost::shared_ptr<SOCKS5ProxiedConnection> ref;

			static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort) {
				return ref(new SOCKS5ProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort));
			}

		private:
			SOCKS5ProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort);

			virtual void initializeProxy();
			virtual void handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data);

		private:
			enum {
				ProxyAuthenticating = 0,
				ProxyConnecting,
			} proxyState_;
	};
}
