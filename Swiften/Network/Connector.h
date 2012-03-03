/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <deque>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Network/DomainNameServiceQuery.h>
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

	class Connector : public boost::bsignals::trackable, public boost::enable_shared_from_this<Connector> {
		public:
			typedef boost::shared_ptr<Connector> ref;

			static Connector::ref create(const std::string& hostname, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, int defaultPort = 5222) {
				return ref(new Connector(hostname, resolver, connectionFactory, timerFactory, defaultPort));
			}

			void setTimeoutMilliseconds(int milliseconds);
			void start();
			void stop();

			boost::signal<void (boost::shared_ptr<Connection>, boost::shared_ptr<Error>)> onConnectFinished;

		private:
			Connector(const std::string& hostname, DomainNameResolver*, ConnectionFactory*, TimerFactory*, int defaultPort);

			void handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result);
			void handleAddressQueryResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error);
			void queryAddress(const std::string& hostname);

			void tryNextServiceOrFallback();
			void tryNextAddress();
			void tryConnect(const HostAddressPort& target);

			void handleConnectionConnectFinished(bool error);
			void finish(boost::shared_ptr<Connection>);
			void handleTimeout();


		private:
			std::string hostname;
			DomainNameResolver* resolver;
			ConnectionFactory* connectionFactory;
			TimerFactory* timerFactory;
			int defaultPort;
			int timeoutMilliseconds;
			boost::shared_ptr<Timer> timer;
			boost::shared_ptr<DomainNameServiceQuery> serviceQuery;
			std::deque<DomainNameServiceQuery::Result> serviceQueryResults;
			boost::shared_ptr<DomainNameAddressQuery> addressQuery;
			std::deque<HostAddress> addressQueryResults;
			bool queriedAllServices;
			boost::shared_ptr<Connection> currentConnection;
			bool foundSomeDNS;
	};
};
