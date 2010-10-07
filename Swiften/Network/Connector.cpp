/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/Connector.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameAddressQuery.h"
#include "Swiften/Network/TimerFactory.h"

namespace Swift {

Connector::Connector(const String& hostname, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory) : hostname(hostname), resolver(resolver), connectionFactory(connectionFactory), timerFactory(timerFactory), timeoutMilliseconds(0), queriedAllServices(true) {
}

void Connector::setTimeoutMilliseconds(int milliseconds) {
	timeoutMilliseconds = milliseconds;
}

void Connector::start() {
	//std::cout << "Connector::start()" << std::endl;
	assert(!currentConnection);
	assert(!serviceQuery);
	assert(!timer);
	queriedAllServices = false;
	serviceQuery = resolver->createServiceQuery("_xmpp-client._tcp." + hostname);
	serviceQuery->onResult.connect(boost::bind(&Connector::handleServiceQueryResult, shared_from_this(), _1));
	if (timeoutMilliseconds > 0) {
		timer = timerFactory->createTimer(timeoutMilliseconds);
		timer->onTick.connect(boost::bind(&Connector::handleTimeout, shared_from_this()));
		timer->start();
	}
	serviceQuery->run();
}

void Connector::stop() {
	finish(boost::shared_ptr<Connection>());
}

void Connector::queryAddress(const String& hostname) {
	assert(!addressQuery);
	addressQuery = resolver->createAddressQuery(hostname);
	addressQuery->onResult.connect(boost::bind(&Connector::handleAddressQueryResult, shared_from_this(), _1, _2));
	addressQuery->run();
}

void Connector::handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result) {
	//std::cout << "Received SRV results" << std::endl;
	serviceQueryResults = std::deque<DomainNameServiceQuery::Result>(result.begin(), result.end());
	serviceQuery.reset();
	tryNextServiceOrFallback();
}

void Connector::tryNextServiceOrFallback() {
	if (queriedAllServices) {
		//std::cout << "Connector::tryNextServiceOrCallback(): Queried all hosts. Error." << std::endl;
		finish(boost::shared_ptr<Connection>());
	}
	else if (serviceQueryResults.empty()) {
		//std::cout << "Connector::tryNextHostName(): Falling back on A resolution" << std::endl;
		// Fall back on simple address resolving
		queriedAllServices = true;
		queryAddress(hostname);
	}
	else {
		//std::cout << "Connector::tryNextHostName(): Querying next address" << std::endl;
		queryAddress(serviceQueryResults.front().hostname);
	}
}

void Connector::handleAddressQueryResult(const std::vector<HostAddress>& addresses, boost::optional<DomainNameResolveError> error) {
	//std::cout << "Connector::handleAddressQueryResult(): Start" << std::endl;
	addressQuery.reset();
	if (error || addresses.empty()) {
		if (!serviceQueryResults.empty()) {
			serviceQueryResults.pop_front();
		}
		tryNextServiceOrFallback();
	}
	else {
		addressQueryResults = std::deque<HostAddress>(addresses.begin(), addresses.end());
		tryNextAddress();
	}
}

void Connector::tryNextAddress() {
	if (addressQueryResults.empty()) {
		//std::cout << "Connector::tryNextAddress(): Done trying addresses. Moving on" << std::endl;
		// Done trying all addresses. Move on to the next host.
		if (!serviceQueryResults.empty()) {
			serviceQueryResults.pop_front();
		}
		tryNextServiceOrFallback();
	}
	else {
		//std::cout << "Connector::tryNextAddress(): trying next address." << std::endl;
		HostAddress address = addressQueryResults.front();
		addressQueryResults.pop_front();

		int port = 5222;
		if (!serviceQueryResults.empty()) {
			port = serviceQueryResults.front().port;
		}

		tryConnect(HostAddressPort(address, port));
	}
}

void Connector::tryConnect(const HostAddressPort& target) {
	assert(!currentConnection);
	//std::cout << "Connector::tryConnect() " << target.getAddress().toString() << " " << target.getPort() << std::endl;
	currentConnection = connectionFactory->createConnection();
	currentConnection->onConnectFinished.connect(boost::bind(&Connector::handleConnectionConnectFinished, shared_from_this(), _1));
	currentConnection->connect(target);
}

void Connector::handleConnectionConnectFinished(bool error) {
	//std::cout << "Connector::handleConnectionConnectFinished() " << error << std::endl;
	if (error) {
		currentConnection.reset();
		if (!addressQueryResults.empty()) {
			tryNextAddress();
		}
		else {
			if (!serviceQueryResults.empty()) {
				serviceQueryResults.pop_front();
			}
			tryNextServiceOrFallback();
		}
	}
	else {
		finish(currentConnection);
	}
}

void Connector::finish(boost::shared_ptr<Connection> connection) {
	if (timer) {
		timer->stop();
		timer->onTick.disconnect(boost::bind(&Connector::handleTimeout, shared_from_this()));
		timer.reset();
	}
	if (serviceQuery) {
		serviceQuery->onResult.disconnect(boost::bind(&Connector::handleServiceQueryResult, shared_from_this(), _1));
		serviceQuery.reset();
	}
	if (addressQuery) {
		addressQuery->onResult.disconnect(boost::bind(&Connector::handleAddressQueryResult, shared_from_this(), _1, _2));
		addressQuery.reset();
	}
	if (currentConnection) {
		currentConnection->onConnectFinished.disconnect(boost::bind(&Connector::handleConnectionConnectFinished, shared_from_this(), _1));
	}
	onConnectFinished(connection);
}

void Connector::handleTimeout() {
	finish(boost::shared_ptr<Connection>());
}

};
