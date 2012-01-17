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

#pragma once

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/SafeString.h>

namespace Swift {
	class DomainNameResolver;
	class TimerFactory;
	class EventLoop;
	class HTTPConnectProxiedConnectionFactory : public ConnectionFactory {
		public:
			HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort);
			HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			DomainNameResolver* resolver_;
			ConnectionFactory* connectionFactory_;
			TimerFactory* timerFactory_;
			EventLoop* eventLoop_;
			std::string proxyHost_;
			int proxyPort_;
			SafeString authID_;
			SafeString authPassword_;
	};
}
