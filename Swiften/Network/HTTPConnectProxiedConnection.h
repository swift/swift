/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#pragma once

#include <Swiften/Network/ProxiedConnection.h>

namespace Swift {
	class DomainNameResolver;
	class ConnectionFactory;
	class EventLoop;
	class TimerFactory;

	class HTTPConnectProxiedConnection : public ProxiedConnection {
		public:
			typedef boost::shared_ptr<HTTPConnectProxiedConnection> ref;

			static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword) {
				return ref(new HTTPConnectProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort, authID, authPassword));
			}

		private:
			HTTPConnectProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword);

			virtual void initializeProxy();
			virtual void handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data);

		private:
			SafeByteArray authID_;
			SafeByteArray authPassword_;
	};
}
