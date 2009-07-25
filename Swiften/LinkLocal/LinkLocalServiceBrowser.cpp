#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

LinkLocalServiceBrowser::LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier) : querier(querier) {
	browseQuery = querier->createBrowseQuery();
	browseQuery->onServiceAdded.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceAdded, this, _1));
	browseQuery->onServiceRemoved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceRemoved, this, _1));
	browseQuery->startBrowsing();
}

LinkLocalServiceBrowser::~LinkLocalServiceBrowser() {
	browseQuery->stopBrowsing();
}

std::vector<LinkLocalService> LinkLocalServiceBrowser::getServices() const {
	std::vector<LinkLocalService> result;
	for (ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		result.push_back(LinkLocalService(i->first, i->second));
	}
	return result;
}

void LinkLocalServiceBrowser::handleServiceAdded(const DNSSDServiceID& service) {
	boost::shared_ptr<DNSSDResolveServiceQuery> resolveQuery = querier->createResolveServiceQuery(service);
	resolveQuery->onServiceResolved.connect(
		boost::bind(&LinkLocalServiceBrowser::handleServiceResolved, this, service, _1));
	std::pair<ResolveQueryMap::iterator, bool> r = resolveQueries.insert(std::make_pair(service, resolveQuery));
	if (!r.second) {
		r.first->second = resolveQuery;
	}
	resolveQuery->start();
}

void LinkLocalServiceBrowser::handleServiceRemoved(const DNSSDServiceID& service) {
	ResolveQueryMap::iterator i = resolveQueries.find(service);
	assert(i != resolveQueries.end());
	i->second->stop();
	resolveQueries.erase(i);
	onServiceRemoved(service);
}

void LinkLocalServiceBrowser::handleServiceResolved(const DNSSDServiceID& service, const boost::optional<DNSSDResolveServiceQuery::Result>& result) {
	if (result) {
		std::pair<ServiceMap::iterator, bool> r = services.insert(std::make_pair(service, *result));
		if (r.second) {
			onServiceAdded(service);
		}
		else {
			r.first->second = *result;
			onServiceChanged(service);
		}
	}
}

}
