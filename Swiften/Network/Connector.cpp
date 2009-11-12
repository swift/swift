#include "Swiften/Network/Connector.h"

#include <boost/bind.hpp>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameResolveException.h"

namespace Swift {

Connector::Connector(const String& hostname, DomainNameResolver* resolver, ConnectionFactory* connectionFactory) : hostname(hostname), resolver(resolver), connectionFactory(connectionFactory) {
}

void Connector::start() {
	assert(!currentConnection);
	try {
		std::vector<HostAddressPort> resolveResult = resolver->resolve(hostname.getUTF8String());
		resolvedHosts = std::deque<HostAddressPort>(resolveResult.begin(), resolveResult.end());
		tryNextHostname();
	}
	catch (const DomainNameResolveException&) {
		onConnectFinished(boost::shared_ptr<Connection>());
	}
}

void Connector::tryNextHostname() {
	if (resolvedHosts.empty()) {
		onConnectFinished(boost::shared_ptr<Connection>());
	}
	else {
		HostAddressPort remote = resolvedHosts.front();
		resolvedHosts.pop_front();
		currentConnection = connectionFactory->createConnection();
		currentConnection->onConnectFinished.connect(boost::bind(&Connector::handleConnectionConnectFinished, this, _1));
		currentConnection->connect(remote);
	}
}

void Connector::handleConnectionConnectFinished(bool error) {
	if (error) {
		tryNextHostname();
	}
	else {
		onConnectFinished(currentConnection);
	}
}

};
