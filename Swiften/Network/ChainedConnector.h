/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/Error.h>

namespace Swift {
	class Connection;
	class Connector;
	class ConnectionFactory;
	class TimerFactory;
	class DomainNameResolver;

	class ChainedConnector {
		public:
			ChainedConnector(const std::string& hostname, DomainNameResolver*, const std::vector<ConnectionFactory*>&, TimerFactory*);

			void setTimeoutMilliseconds(int milliseconds);
			void start();
			void stop();

			boost::signal<void (boost::shared_ptr<Connection>, boost::shared_ptr<Error>)> onConnectFinished;

		private:
			void finish(boost::shared_ptr<Connection> connection, boost::shared_ptr<Error>);
			void tryNextConnectionFactory();
			void handleConnectorFinished(boost::shared_ptr<Connection>, boost::shared_ptr<Error>);

		private:
			std::string hostname;
			DomainNameResolver* resolver;
			std::vector<ConnectionFactory*> connectionFactories;
			TimerFactory* timerFactory;
			int timeoutMilliseconds;
			std::deque<ConnectionFactory*> connectionFactoryQueue;
			boost::shared_ptr<Connector> currentConnector;
			boost::shared_ptr<Error> lastError;
	};
};
