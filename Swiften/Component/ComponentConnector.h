/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <deque>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/HostAddressPort.h>
#include <string>
#include <Swiften/Network/DomainNameResolveError.h>

namespace Swift {
	class DomainNameAddressQuery;
	class DomainNameResolver;
	class ConnectionFactory;
	class TimerFactory;

	class ComponentConnector : public boost::bsignals::trackable, public boost::enable_shared_from_this<ComponentConnector> {
		public:
			typedef boost::shared_ptr<ComponentConnector> ref;

			static ComponentConnector::ref create(const std::string& hostname, int port, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory) {
				return ref(new ComponentConnector(hostname, port, resolver, connectionFactory, timerFactory));
			}

			void setTimeoutMilliseconds(int milliseconds);

			void start();
			void stop();

			boost::signal<void (boost::shared_ptr<Connection>)> onConnectFinished;

		private:
			ComponentConnector(const std::string& hostname, int port, DomainNameResolver*, ConnectionFactory*, TimerFactory*);

			void handleAddressQueryResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error);
			void tryNextAddress();
			void tryConnect(const HostAddressPort& target);

			void handleConnectionConnectFinished(bool error);
			void finish(boost::shared_ptr<Connection>);
			void handleTimeout();


		private:
			std::string hostname;
			int port;
			DomainNameResolver* resolver;
			ConnectionFactory* connectionFactory;
			TimerFactory* timerFactory;
			int timeoutMilliseconds;
			boost::shared_ptr<Timer> timer;
			boost::shared_ptr<DomainNameAddressQuery> addressQuery;
			std::deque<HostAddress> addressQueryResults;
			boost::shared_ptr<Connection> currentConnection;
	};
};
