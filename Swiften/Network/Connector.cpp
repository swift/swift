#include "Swiften/Network/Connector.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameAddressQuery.h"

namespace Swift {

Connector::Connector(const String& hostname, DomainNameResolver* resolver, ConnectionFactory* connectionFactory) : hostname(hostname), resolver(resolver), connectionFactory(connectionFactory), queriedAllHosts(true) {
}

void Connector::start() {
	assert(!currentConnection);
	assert(!serviceQuery);
	queriedAllHosts = false;
	serviceQuery = resolver->createServiceQuery("_xmpp-client._tcp." + hostname);
	serviceQuery->onResult.connect(boost::bind(&Connector::handleServiceQueryResult, this, _1));
	serviceQuery->run();
}

void Connector::queryAddress(const String& hostname) {
	assert(!addressQuery);
	addressQuery = resolver->createAddressQuery(hostname);
	addressQuery->onResult.connect(boost::bind(&Connector::handleAddressQueryResult, this, _1, _2));
	addressQuery->run();
}

void Connector::handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result) {
	serviceQueryResults = std::deque<DomainNameServiceQuery::Result>(result.begin(), result.end());
	serviceQuery.reset();
	tryNextHostname();
}

void Connector::tryNextHostname() {
	if (queriedAllHosts) {
		onConnectFinished(boost::shared_ptr<Connection>());
	}
	else if (serviceQueryResults.empty()) {
		// Fall back on simple address resolving
		queriedAllHosts = true;
		queryAddress(hostname);
	}
	else {
		queryAddress(serviceQueryResults.front().hostname);
	}
}

void Connector::handleAddressQueryResult(const HostAddress& address, boost::optional<DomainNameResolveError> error) {
	addressQuery.reset();
	if (!serviceQueryResults.empty()) {
		DomainNameServiceQuery::Result serviceQueryResult = serviceQueryResults.front();
		serviceQueryResults.pop_front();
		if (error) {
			tryNextHostname();
		}
		else {
			tryConnect(HostAddressPort(address, serviceQueryResult.port));
		}
	}
	else if (error) {
		// The fallback address query failed
		assert(queriedAllHosts);
		onConnectFinished(boost::shared_ptr<Connection>());
	}
	else {
		// The fallback query succeeded
		tryConnect(HostAddressPort(address, 5222));
	}
}

void Connector::tryConnect(const HostAddressPort& target) {
	assert(!currentConnection);
	currentConnection = connectionFactory->createConnection();
	currentConnection->onConnectFinished.connect(boost::bind(&Connector::handleConnectionConnectFinished, this, _1));
	currentConnection->connect(target);
}

void Connector::handleConnectionConnectFinished(bool error) {
	if (error) {
		currentConnection.reset();
		tryNextHostname();
	}
	else {
		onConnectFinished(currentConnection);
	}
}

};
