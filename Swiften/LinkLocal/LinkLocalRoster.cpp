#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalRoster.h"

namespace Swift {

LinkLocalRoster::LinkLocalRoster(boost::shared_ptr<DNSSDService> service) : dnsSDService(service) {
	service->onServiceAdded.connect(boost::bind(&LinkLocalRoster::handleServiceAdded, this, _1));
	service->onServiceRemoved.connect(boost::bind(&LinkLocalRoster::handleServiceRemoved, this, _1));
}

void LinkLocalRoster::handleServiceAdded(const DNSSDService::Service& service) {
	std::cout << "Service added " << service.name << " " << service.type << " " << service.domain << std::endl;
}

void LinkLocalRoster::handleServiceRemoved(const DNSSDService::Service& service) {
	std::cout << "Service removed " << service.name << " " << service.type << " " << service.domain << std::endl;
}

}
