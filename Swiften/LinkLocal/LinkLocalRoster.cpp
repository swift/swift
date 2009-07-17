#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalRoster.h"

namespace Swift {

LinkLocalRoster::LinkLocalRoster(boost::shared_ptr<DNSSDService> service) : dnsSDService(service) {
	dnsSDService->onServiceAdded.connect(boost::bind(&LinkLocalRoster::handleServiceAdded, this, _1));
	dnsSDService->onServiceRemoved.connect(boost::bind(&LinkLocalRoster::handleServiceRemoved, this, _1));
	dnsSDService->onServiceResolved.connect(boost::bind(&LinkLocalRoster::handleServiceResolved, this, _1, _2));
	dnsSDService->onError.connect(boost::bind(&LinkLocalRoster::handleDNSSDError, this));
	dnsSDService->onServiceRegistered.connect(boost::bind(&LinkLocalRoster::handleServiceRegistered, this, _1));
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
}

void LinkLocalRoster::handleServiceRegistered(const DNSSDService::Service& service) {
	selfService = service;
}

void LinkLocalRoster::handleDNSSDError() {
	std::cout << "DNSSD Error" << std::endl;
}

}
