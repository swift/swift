/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/HostNameOrAddress.h>

namespace Swift {
	class DomainNameResolver;
	class TimerFactory;

	class SOCKS5ProxiedConnectionFactory : public ConnectionFactory {
		public:
			SOCKS5ProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			DomainNameResolver* resolver_;
			ConnectionFactory* connectionFactory_;
			TimerFactory* timerFactory_;
			std::string proxyHost_;
			int proxyPort_;
	};
}
