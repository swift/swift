#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

LinkLocalServiceBrowser::LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier) : querier(querier) {
  /*dnsSDService = factory->createDNSSDService();
	dnsSDService->onServiceAdded.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceAdded, this, _1));
	dnsSDService->onServiceRemoved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceRemoved, this, _1));
	dnsSDService->onServiceResolved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceResolved, this, _1, _2));*/
}

std::vector<LinkLocalService> LinkLocalServiceBrowser::getServices() const {
	std::vector<LinkLocalService> result;
	/*for (ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		result.push_back(LinkLocalService(i->first, i->second));
	}*/
	return result;
}

/*void LinkLocalServiceBrowser::handleServiceAdded(const DNSSDServiceID& service) {
	dnsSDService->startResolvingService(service);
}

void LinkLocalServiceBrowser::handleServiceRemoved(const DNSSDServiceID& service) {
	dnsSDService->stopResolvingService(service);
	services.erase(service);
	onServiceRemoved(service);
}

void LinkLocalServiceBrowser::handleServiceResolved(const DNSSDServiceID& service, const DNSSDService::ResolveResult& result) {
	std::pair<ServiceMap::iterator, bool> r = services.insert(std::make_pair(service, result));
	if (r.second) {
		onServiceAdded(service);
	}
	else {
		r.first->second = result;
		onServiceChanged(service);
	}
}
*/

}
