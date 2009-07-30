#include "Swiften/LinkLocal/LinkLocalConnector.h"

#include <boost/bind.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDQuerier.h"

namespace Swift {

LinkLocalConnector::LinkLocalConnector(
		const JID& remoteJID,
		const String& hostname,
		int port,
		boost::shared_ptr<DNSSDQuerier> querier,
		boost::shared_ptr<Connection> connection) : 
			remoteJID_(remoteJID),
			hostname_(hostname),
			port_(port),
			querier_(querier),
			connection_(connection),
			resolving_(false) {
}

void LinkLocalConnector::connect() {
	resolving_ = true;
	//querier_->onHostnameResolved.connect(boost::bind(&LinkLocalConnector::handleHostnameResolved, boost::dynamic_pointer_cast<LinkLocalConnector>(shared_from_this()), _1, _2));
	//querier_->resolveHostname(hostname_);
}

/*
void LinkLocalConnector::handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address) {
	if (resolving_) {
		if (hostname == hostname_) {
			resolving_ = false;
			if (address) {
				connection_->onConnectFinished.connect(boost::bind(boost::ref(onConnectFinished), _1));
				connection_->connect(HostAddressPort(*address, port_));
			}
			else {
				onConnectFinished(false);
			}
		}
	}
}
*/

void LinkLocalConnector::handleConnected(bool error) {
	onConnectFinished(error);
}

void LinkLocalConnector::queueElement(boost::shared_ptr<Element> element) {
	queuedElements_.push_back(element);
}


}
