/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/ChainedConnector.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Network/Connector.h>

using namespace Swift;

ChainedConnector::ChainedConnector(
		const std::string& hostname, 
		DomainNameResolver* resolver, 
		const std::vector<ConnectionFactory*>& connectionFactories, 
		TimerFactory* timerFactory) : 
			hostname(hostname), 
			resolver(resolver), 
			connectionFactories(connectionFactories), 
			timerFactory(timerFactory), 
			timeoutMilliseconds(0) {
}

void ChainedConnector::setTimeoutMilliseconds(int milliseconds) {
	timeoutMilliseconds = milliseconds;
}

void ChainedConnector::start() {
	SWIFT_LOG(debug) << "Starting queued connector for " << hostname << std::endl;

	connectionFactoryQueue = std::deque<ConnectionFactory*>(connectionFactories.begin(), connectionFactories.end());
	tryNextConnectionFactory();
}

void ChainedConnector::stop() {
	if (currentConnector) {
		currentConnector->onConnectFinished.disconnect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1));
		currentConnector->stop();
		currentConnector.reset();
	}
	finish(boost::shared_ptr<Connection>());
}

void ChainedConnector::tryNextConnectionFactory() {
	SWIFT_LOG(debug) << "Trying next connection factory" << std::endl;
	assert(!currentConnector);
	if (connectionFactoryQueue.empty()) {
		SWIFT_LOG(debug) << "No more connection factories" << std::endl;
		finish(boost::shared_ptr<Connection>());
	}
	else {
		ConnectionFactory* connectionFactory = connectionFactoryQueue.front();
		connectionFactoryQueue.pop_front();
		currentConnector = Connector::create(hostname, resolver, connectionFactory, timerFactory);
		currentConnector->setTimeoutMilliseconds(timeoutMilliseconds);
		currentConnector->onConnectFinished.connect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1));
		currentConnector->start();
	}
}

void ChainedConnector::handleConnectorFinished(boost::shared_ptr<Connection> connection) {
	SWIFT_LOG(debug) << "Connector finished" << std::endl;
	currentConnector->onConnectFinished.disconnect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1));
	currentConnector.reset();
	if (connection) {
		finish(connection);
	}
	else {
		tryNextConnectionFactory();
	}
}

void ChainedConnector::finish(boost::shared_ptr<Connection> connection) {
	onConnectFinished(connection);
}
