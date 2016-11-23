/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/ChainedConnector.h>

#include <typeinfo>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/Connector.h>

using namespace Swift;

ChainedConnector::ChainedConnector(
        const std::string& hostname,
        int port,
        const boost::optional<std::string>& serviceLookupPrefix,
        DomainNameResolver* resolver,
        const std::vector<ConnectionFactory*>& connectionFactories,
        TimerFactory* timerFactory) :
            hostname(hostname),
            port(port),
            serviceLookupPrefix(serviceLookupPrefix),
            resolver(resolver),
            connectionFactories(connectionFactories),
            timerFactory(timerFactory),
            timeoutMilliseconds(0) {
}

ChainedConnector::~ChainedConnector() {
    if (currentConnector) {
        currentConnector->onConnectFinished.disconnect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1, _2));
        currentConnector->stop();
        currentConnector.reset();
    }
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
        currentConnector->onConnectFinished.disconnect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1, _2));
        currentConnector->stop();
        currentConnector.reset();
    }
    finish(std::shared_ptr<Connection>(), std::shared_ptr<Error>());
}

void ChainedConnector::tryNextConnectionFactory() {
    assert(!currentConnector);
    if (connectionFactoryQueue.empty()) {
        SWIFT_LOG(debug) << "No more connection factories" << std::endl;
        finish(std::shared_ptr<Connection>(), lastError);
    }
    else {
        ConnectionFactory* connectionFactory = connectionFactoryQueue.front();
        SWIFT_LOG(debug) << "Trying next connection factory: " << typeid(*connectionFactory).name() << std::endl;
        connectionFactoryQueue.pop_front();
        currentConnector = Connector::create(hostname, port, serviceLookupPrefix, resolver, connectionFactory, timerFactory);
        currentConnector->setTimeoutMilliseconds(timeoutMilliseconds);
        currentConnector->onConnectFinished.connect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1, _2));
        currentConnector->start();
    }
}

void ChainedConnector::handleConnectorFinished(std::shared_ptr<Connection> connection, std::shared_ptr<Error> error) {
    SWIFT_LOG(debug) << "Connector finished" << std::endl;
    currentConnector->onConnectFinished.disconnect(boost::bind(&ChainedConnector::handleConnectorFinished, this, _1, _2));
    lastError = error;
    currentConnector.reset();
    if (connection) {
        finish(connection, error);
    }
    else {
        tryNextConnectionFactory();
    }
}

void ChainedConnector::finish(std::shared_ptr<Connection> connection, std::shared_ptr<Error> error) {
    onConnectFinished(connection, error);
}
