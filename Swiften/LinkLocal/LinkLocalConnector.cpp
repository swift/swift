#include "Swiften/LinkLocal/LinkLocalConnector.h"

#include <boost/bind.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h"

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
	resolveQuery->onHostnameResolved.connect(boost::bind(
			&LinkLocalConnector::handleHostnameResolved, 
			boost::dynamic_pointer_cast<LinkLocalConnector>(shared_from_this()), 
			_1));
	resolveQuery->run();
}

void LinkLocalConnector::cancel() {
	if (resolveQuery) {
		resolveQuery->finish();
	}
	resolveQuery.reset();
	connection->disconnect();
}

void LinkLocalConnector::handleHostnameResolved(const boost::optional<HostAddress>& address) {
	if (address) {
		resolveQuery->finish();
		resolveQuery.reset();
		connection->onConnectFinished.connect(
				boost::bind(boost::ref(onConnectFinished), _1));
		connection->connect(HostAddressPort(*address, service.getPort()));
	}
	else {
		onConnectFinished(true);
	}
}

void LinkLocalConnector::handleConnected(bool error) {
	onConnectFinished(error);
}

void LinkLocalConnector::queueElement(boost::shared_ptr<Element> element) {
	queuedElements.push_back(element);
}


}
