/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/ComponentConnector.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {

ComponentConnector::ComponentConnector(const std::string& hostname, int port, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory) : hostname(hostname), port(port), resolver(resolver), connectionFactory(connectionFactory), timerFactory(timerFactory), timeoutMilliseconds(0) {
}

void ComponentConnector::setTimeoutMilliseconds(int milliseconds) {
	timeoutMilliseconds = milliseconds;
}

void ComponentConnector::start() {
	assert(!currentConnection);
	assert(!timer);
	assert(!addressQuery);
	addressQuery = resolver->createAddressQuery(hostname);
	addressQuery->onResult.connect(boost::bind(&ComponentConnector::handleAddressQueryResult, shared_from_this(), _1, _2));
	if (timeoutMilliseconds > 0) {
		timer = timerFactory->createTimer(timeoutMilliseconds);
		timer->onTick.connect(boost::bind(&ComponentConnector::handleTimeout, shared_from_this()));
		timer->start();
	}
	addressQuery->run();
}

void ComponentConnector::stop() {
	finish(boost::shared_ptr<Connection>());
}


void ComponentConnector::handleAddressQueryResult(const std::vector<HostAddress>& addresses, boost::optional<DomainNameResolveError> error) {
	addressQuery.reset();
	if (error || addresses.empty()) {
		finish(boost::shared_ptr<Connection>());
	}
	else {
		addressQueryResults = std::deque<HostAddress>(addresses.begin(), addresses.end());
		tryNextAddress();
	}
}

void ComponentConnector::tryNextAddress() {
	assert(!addressQueryResults.empty());
	HostAddress address = addressQueryResults.front();
	addressQueryResults.pop_front();
	tryConnect(HostAddressPort(address, port));
}

void ComponentConnector::tryConnect(const HostAddressPort& target) {
	assert(!currentConnection);
	currentConnection = connectionFactory->createConnection();
	currentConnection->onConnectFinished.connect(boost::bind(&ComponentConnector::handleConnectionConnectFinished, shared_from_this(), _1));
	currentConnection->connect(target);
}

void ComponentConnector::handleConnectionConnectFinished(bool error) {
	currentConnection->onConnectFinished.disconnect(boost::bind(&ComponentConnector::handleConnectionConnectFinished, shared_from_this(), _1));
	if (error) {
		currentConnection.reset();
		if (!addressQueryResults.empty()) {
			tryNextAddress();
		}
		else {
			finish(boost::shared_ptr<Connection>());
		}
	}
	else {
		finish(currentConnection);
	}
}

void ComponentConnector::finish(boost::shared_ptr<Connection> connection) {
	if (timer) {
		timer->stop();
		timer->onTick.disconnect(boost::bind(&ComponentConnector::handleTimeout, shared_from_this()));
		timer.reset();
	}
	if (addressQuery) {
		addressQuery->onResult.disconnect(boost::bind(&ComponentConnector::handleAddressQueryResult, shared_from_this(), _1, _2));
		addressQuery.reset();
	}
	if (currentConnection) {
		currentConnection->onConnectFinished.disconnect(boost::bind(&ComponentConnector::handleConnectionConnectFinished, shared_from_this(), _1));
		currentConnection.reset();
	}
	onConnectFinished(connection);
}

void ComponentConnector::handleTimeout() {
	finish(boost::shared_ptr<Connection>());
}

};
