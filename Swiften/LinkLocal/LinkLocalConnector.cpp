/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/LinkLocalConnector.h>

#include <boost/bind.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>

namespace Swift {

LinkLocalConnector::LinkLocalConnector(
		const LinkLocalService& service,
		boost::shared_ptr<DNSSDQuerier> querier,
		boost::shared_ptr<Connection> connection) : 
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
			boost::dynamic_pointer_cast<LinkLocalConnector>(shared_from_this()), 
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

void LinkLocalConnector::queueElement(boost::shared_ptr<Element> element) {
	queuedElements.push_back(element);
}


}
