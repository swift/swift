/*
 * Copyright (c) 2012-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/SafeString.h>

namespace Swift {
	class DomainNameResolver;
	class TimerFactory;
	class EventLoop;
	class HTTPTrafficFilter;

	class SWIFTEN_API HTTPConnectProxiedConnectionFactory : public ConnectionFactory {
		public:
			HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, boost::shared_ptr<HTTPTrafficFilter> httpTrafficFilter = boost::shared_ptr<HTTPTrafficFilter>());
			HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword, boost::shared_ptr<HTTPTrafficFilter> httpTrafficFilter = boost::shared_ptr<HTTPTrafficFilter>());

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			DomainNameResolver* resolver_;
			ConnectionFactory* connectionFactory_;
			TimerFactory* timerFactory_;
			std::string proxyHost_;
			int proxyPort_;
			SafeString authID_;
			SafeString authPassword_;
			boost::shared_ptr<HTTPTrafficFilter> httpTrafficFilter_;
	};
}
