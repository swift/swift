#pragma once

#include <deque>
#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/Base/String.h"
#include "Swiften/Network/DomainNameResolveError.h"

namespace Swift {
	class DomainNameAddressQuery;
	class DomainNameResolver;
	class ConnectionFactory;

	class Connector : public boost::bsignals::trackable {
		public:
			Connector(const String& hostname, DomainNameResolver*, ConnectionFactory*);

			void setTimeoutMilliseconds(int milliseconds);
			void start();

			boost::signal<void (boost::shared_ptr<Connection>)> onConnectFinished;

		private:
			void handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result);
			void handleAddressQueryResult(const HostAddress& address, boost::optional<DomainNameResolveError> error);
			void queryAddress(const String& hostname);

			void tryNextHostname();
			void tryConnect(const HostAddressPort& target);

			void handleConnectionConnectFinished(bool error);

		private:
			String hostname;
			DomainNameResolver* resolver;
			ConnectionFactory* connectionFactory;
			int timeoutMilliseconds;
			boost::shared_ptr<DomainNameServiceQuery> serviceQuery;
			std::deque<DomainNameServiceQuery::Result> serviceQueryResults;
			boost::shared_ptr<DomainNameAddressQuery> addressQuery;
			bool queriedAllHosts;
			boost::shared_ptr<Connection> currentConnection;
	};
};
