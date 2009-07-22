#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

LinkLocalServiceBrowser::LinkLocalServiceBrowser(boost::shared_ptr<DNSSDService> service) : dnsSDService(service) {
	dnsSDService->onServiceAdded.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceAdded, this, _1));
	dnsSDService->onServiceRemoved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceRemoved, this, _1));
	dnsSDService->onServiceResolved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceResolved, this, _1, _2));
}

void LinkLocalServiceBrowser::handleServiceAdded(const LinkLocalServiceID& service) {
	dnsSDService->startResolvingService(service);
}

void LinkLocalServiceBrowser::handleServiceRemoved(const LinkLocalServiceID& service) {
	/*dnsSDService->stopResolvingService(service);
	services.erase(service);*/
}

void LinkLocalServiceBrowser::handleServiceResolved(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& result) {
	std::pair<ServiceMap::iterator, bool> r = services.insert(std::make_pair(service, result));
	if (r.second) {
		onServiceAdded(service);
	}
	else {
		onServiceChanged(service);
	}
}


}
