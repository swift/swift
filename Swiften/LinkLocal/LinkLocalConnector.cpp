/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/LinkLocalConnector.h>

#include <boost/bind.hpp>

#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {

LinkLocalConnector::LinkLocalConnector(
        const LinkLocalService& service,
        std::shared_ptr<DNSSDQuerier> querier,
        std::shared_ptr<Connection> connection) :
            service(service),
            querier(querier),
            connection(connection) {
}

LinkLocalConnector::~LinkLocalConnector() {
    assert(!resolveQuery);
}

void LinkLocalConnector::connect() {
    resolveQuery = querier->createResolveHostnameQuery(
            service.getHostname(),
            service.getID().getNetworkInterfaceID());
    resolveQueryHostNameResolvedConnection = resolveQuery->onHostnameResolved.connect(boost::bind(
            &LinkLocalConnector::handleHostnameResolved,
            std::dynamic_pointer_cast<LinkLocalConnector>(shared_from_this()),
            _1));
    resolveQuery->run();
}

void LinkLocalConnector::cancel() {
    if (resolveQuery) {
        resolveQuery->finish();
        resolveQueryHostNameResolvedConnection.disconnect();
        resolveQuery.reset();
    }
    connectionConnectFinishedConnection.disconnect();
    connection->disconnect();
}

void LinkLocalConnector::handleHostnameResolved(const boost::optional<HostAddress>& address) {
    resolveQuery->finish();
    resolveQueryHostNameResolvedConnection.disconnect();
    resolveQuery.reset();
    if (address) {
        connectionConnectFinishedConnection = connection->onConnectFinished.connect(
                boost::bind(&LinkLocalConnector::handleConnected, shared_from_this(), _1));
        connection->connect(HostAddressPort(*address, service.getPort()));
    }
    else {
        onConnectFinished(true);
    }
}

void LinkLocalConnector::handleConnected(bool error) {
    onConnectFinished(error);
    assert(connectionConnectFinishedConnection.connected());
    connectionConnectFinishedConnection.disconnect();
}

void LinkLocalConnector::queueElement(std::shared_ptr<ToplevelElement> element) {
    queuedElements.push_back(element);
}


}
