/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/LinkLocal/LinkLocalServiceBrowser.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {

LinkLocalServiceBrowser::LinkLocalServiceBrowser(boost::shared_ptr<DNSSDQuerier> querier) : querier(querier), haveError(false) {
}

LinkLocalServiceBrowser::~LinkLocalServiceBrowser() {
	if (isRunning()) {
		std::cerr << "WARNING: LinkLocalServiceBrowser still running on destruction" << std::endl;
	}
}


void LinkLocalServiceBrowser::start() {
	assert(!isRunning());
	haveError = false;
	browseQuery = querier->createBrowseQuery();
	browseQuery->onServiceAdded.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceAdded, this, _1));
	browseQuery->onServiceRemoved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceRemoved, this, _1));
	browseQuery->onError.connect(
			boost::bind(&LinkLocalServiceBrowser::handleBrowseError, this));
	browseQuery->startBrowsing();
}

void LinkLocalServiceBrowser::stop() {
	assert(isRunning());
	if (isRegistered()) {
		unregisterService();
	}
	for (ResolveQueryMap::const_iterator i = resolveQueries.begin(); i != resolveQueries.end(); ++i) {
		i->second->stop();
	}
	resolveQueries.clear();
	services.clear();
	browseQuery->stopBrowsing();
	browseQuery.reset();
	onStopped(haveError);
}

bool LinkLocalServiceBrowser::isRunning() const {
	return browseQuery;
}

bool LinkLocalServiceBrowser::hasError() const {
	return haveError;
}

bool LinkLocalServiceBrowser::isRegistered() const {
	return registerQuery;
}

void LinkLocalServiceBrowser::registerService(const std::string& name, int port, const LinkLocalServiceInfo& info) {
	assert(!registerQuery);
	registerQuery = querier->createRegisterQuery(name, port, info.toTXTRecord());
	registerQuery->onRegisterFinished.connect(
		boost::bind(&LinkLocalServiceBrowser::handleRegisterFinished, this, _1));
	registerQuery->registerService();
}

void LinkLocalServiceBrowser::updateService(const LinkLocalServiceInfo& info) {
	assert(registerQuery);
	registerQuery->updateServiceInfo(info.toTXTRecord());
}

void LinkLocalServiceBrowser::unregisterService() {
	assert(registerQuery);
	registerQuery->unregisterService();
	registerQuery.reset();
	selfService.reset();
}

std::vector<LinkLocalService> LinkLocalServiceBrowser::getServices() const {
	std::vector<LinkLocalService> result;
	for (ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		result.push_back(LinkLocalService(i->first, i->second));
	}
	return result;
}

void LinkLocalServiceBrowser::handleServiceAdded(const DNSSDServiceID& service) {
	if (selfService && service == *selfService) {
		return;
	}

	std::pair<ResolveQueryMap::iterator, bool> r = resolveQueries.insert(std::make_pair(service, boost::shared_ptr<DNSSDResolveServiceQuery>()));
	if (r.second) {
		// There was no existing query yet. Start a new query.
		boost::shared_ptr<DNSSDResolveServiceQuery> resolveQuery = querier->createResolveServiceQuery(service);
		resolveQuery->onServiceResolved.connect(
			boost::bind(&LinkLocalServiceBrowser::handleServiceResolved, this, service, _1));
		r.first->second = resolveQuery;
		resolveQuery->start();
	}
}

void LinkLocalServiceBrowser::handleServiceRemoved(const DNSSDServiceID& service) {
	ResolveQueryMap::iterator i = resolveQueries.find(service);
	if (i == resolveQueries.end()) {
		// Can happen after an unregister(), when getting the old 'self' 
		// service remove notification.
		return;
	}
	i->second->stop();
	resolveQueries.erase(i);
	ServiceMap::iterator j = services.find(service);
	assert(j != services.end());
	LinkLocalService linkLocalService(j->first, j->second);
	services.erase(j);
	onServiceRemoved(linkLocalService);
}

void LinkLocalServiceBrowser::handleServiceResolved(const DNSSDServiceID& service, const boost::optional<DNSSDResolveServiceQuery::Result>& result) {
	if (result) {
		std::pair<ServiceMap::iterator, bool> r = services.insert(std::make_pair(service, *result));
		if (r.second) {
			onServiceAdded(LinkLocalService(r.first->first, r.first->second));
		}
		else {
			r.first->second = *result;
			onServiceChanged(LinkLocalService(r.first->first, r.first->second));
		}
	}
}

void LinkLocalServiceBrowser::handleRegisterFinished(const boost::optional<DNSSDServiceID>& result) {
	if (result) {
		selfService = result;
		onServiceRegistered(*result);
	}
	else {
		haveError = true;
		stop();
	}
}

void LinkLocalServiceBrowser::handleBrowseError() {
	haveError = true;
	stop();
}

}
