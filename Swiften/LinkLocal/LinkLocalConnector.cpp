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
		const JID& remoteJID,
		const String& hostname,
		int interfaceIndex,
		int port,
		boost::shared_ptr<DNSSDQuerier> querier,
		boost::shared_ptr<Connection> connection) : 
			remoteJID(remoteJID),
			hostname(hostname),
			interfaceIndex(interfaceIndex),
			port(port),
			querier(querier),
			connection(connection) {
}

void LinkLocalConnector::connect() {
	resolveQuery = querier->createResolveHostnameQuery(hostname, interfaceIndex);
	resolveQuery->onHostnameResolved.connect(boost::bind(
			&LinkLocalConnector::handleHostnameResolved, 
			boost::dynamic_pointer_cast<LinkLocalConnector>(shared_from_this()), 
			_1));
	resolveQuery->run();
}

void LinkLocalConnector::handleHostnameResolved(const boost::optional<HostAddress>& address) {
	if (address) {
		resolveQuery->finish();
		resolveQuery.reset();
		connection->onConnectFinished.connect(
				boost::bind(boost::ref(onConnectFinished), _1));
		connection->connect(HostAddressPort(*address, port));
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
