#pragma once

#include <deque>
#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class DomainNameResolver;
	class ConnectionFactory;

	class Connector : public boost::bsignals::trackable {
		public:
			Connector(const String& hostname, DomainNameResolver*, ConnectionFactory*);

			void start();

			boost::signal<void (boost::shared_ptr<Connection>)> onConnectFinished;

		private:
			void tryNextHostname();
			void handleConnectionConnectFinished(bool error);

		private:
			String hostname;
			DomainNameResolver* resolver;
			ConnectionFactory* connectionFactory;
			std::deque<HostAddressPort> resolvedHosts;
			boost::shared_ptr<Connection> currentConnection;
	};
		
};
