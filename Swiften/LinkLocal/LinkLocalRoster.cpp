#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalRoster.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

LinkLocalRoster::LinkLocalRoster(boost::shared_ptr<DNSSDService> service) : dnsSDService(service) {
	dnsSDService->onStopped.connect(boost::bind(&LinkLocalRoster::handleStopped, this, _1));
	dnsSDService->onServiceRegistered.connect(boost::bind(&LinkLocalRoster::handleServiceRegistered, this, _1));
	dnsSDService->onServiceAdded.connect(boost::bind(&LinkLocalRoster::handleServiceAdded, this, _1));
	dnsSDService->onServiceRemoved.connect(boost::bind(&LinkLocalRoster::handleServiceRemoved, this, _1));
	dnsSDService->onServiceResolved.connect(boost::bind(&LinkLocalRoster::handleServiceResolved, this, _1, _2));
	dnsSDService->onHostnameResolved.connect(boost::bind(&LinkLocalRoster::handleHostnameResolved, this, _1, _2));
}

void LinkLocalRoster::handleServiceAdded(const DNSSDService::Service& service) {
	if (selfService && *selfService == service) {
		return;
	}
	std::cout << "Service added " << service.name << " " << service.type << " " << service.domain << std::endl;
	dnsSDService->startResolvingService(service);
}

void LinkLocalRoster::handleServiceRemoved(const DNSSDService::Service& service) {
	std::cout << "Service removed " << service.name << " " << service.type << " " << service.domain << std::endl;
	dnsSDService->stopResolvingService(service);
}

void LinkLocalRoster::handleServiceResolved(const DNSSDService::Service& service, const DNSSDService::ResolveResult& result) {
	std::cout << "Service resolved: " << service.name << "->" << result.host << " " << result.port << " " << result.info.getLastName() << std::endl;
	dnsSDService->resolveHostname(result.host);
}

void LinkLocalRoster::handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address) {
	if (address) {
		std::cout << "Address resolved: " << hostname << " " << address->toString() << std::endl;
	}
	else {
		std::cout << "Unable to resolve address for " << hostname << std::endl;
	}
}


void LinkLocalRoster::handleServiceRegistered(const DNSSDService::Service& service) {
	selfService = service;
}

void LinkLocalRoster::handleStopped(bool error) {
	std::cout << "DNSSDService stopped: " << error << std::endl;
}

}
