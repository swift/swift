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
}

boost::shared_ptr<RosterPayload> LinkLocalRoster::getRoster() const {
	boost::shared_ptr<RosterPayload> roster(new RosterPayload());
	for(ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		roster->addItem(getRosterItem(i->first, i->second));
	}
	return roster;
}

std::vector<boost::shared_ptr<Presence> > LinkLocalRoster::getAllPresence() const {
	std::vector<boost::shared_ptr<Presence> > result;
	for(ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		result.push_back(getPresence(i->first, i->second));
	}
	return result;
}

RosterItemPayload LinkLocalRoster::getRosterItem(const DNSSDService::Service& service, const DNSSDService::ResolveResult& resolveResult) const {
 return RosterItemPayload(getJIDForService(service), getRosterName(service, resolveResult), RosterItemPayload::Both);
}

String LinkLocalRoster::getRosterName(const DNSSDService::Service& service, const DNSSDService::ResolveResult& resolveResult) const {
	if (!resolveResult.info.getNick().isEmpty()) {
		return resolveResult.info.getNick();
	}
	else if (!resolveResult.info.getFirstName().isEmpty()) {
		String result = resolveResult.info.getFirstName();
		if (!resolveResult.info.getLastName().isEmpty()) {
			result += " " + resolveResult.info.getLastName();
		}
		return result;
	}
	else if (!resolveResult.info.getLastName().isEmpty()) {
		return resolveResult.info.getLastName();
	}
	return service.name;
}

JID LinkLocalRoster::getJIDForService(const DNSSDService::Service& service) const {
	return JID(service.name);
}

boost::shared_ptr<Presence> LinkLocalRoster::getPresence(const DNSSDService::Service& service, const DNSSDService::ResolveResult& resolveResult) const {
	boost::shared_ptr<Presence> presence(new Presence());
	presence->setFrom(getJIDForService(service));
	switch (resolveResult.info.getStatus()) {
		case LinkLocalServiceInfo::Available:
			presence->setShow(StatusShow::Online);
			break;
		case LinkLocalServiceInfo::Away:
			presence->setShow(StatusShow::Away);
			break;
		case LinkLocalServiceInfo::DND:
			presence->setShow(StatusShow::DND);
			break;
	}
	presence->setStatus(resolveResult.info.getMessage());
	return presence;
}

void LinkLocalRoster::handleServiceAdded(const DNSSDService::Service& service) {
	if (selfService && *selfService == service) {
		return;
	}
	std::cout << "Service added: " << service.name << std::endl;
	dnsSDService->startResolvingService(service);
}

void LinkLocalRoster::handleServiceRemoved(const DNSSDService::Service& service) {
	if (selfService && *selfService == service) {
		return;
	}
	dnsSDService->stopResolvingService(service);
	services.erase(service);
	boost::shared_ptr<RosterPayload> roster(new RosterPayload());
	roster->addItem(RosterItemPayload(getJIDForService(service), "", RosterItemPayload::Remove));
	onRosterChanged(roster);
}

void LinkLocalRoster::handleServiceResolved(const DNSSDService::Service& service, const DNSSDService::ResolveResult& result) {
	services.insert(std::make_pair(service, result));
	std::cout << "Service resolved: " << service.name << std::endl;

	boost::shared_ptr<RosterPayload> roster(new RosterPayload());
	roster->addItem(getRosterItem(service, result));
	onRosterChanged(roster);
	onPresenceChanged(getPresence(service, result));
}

void LinkLocalRoster::handleServiceRegistered(const DNSSDService::Service& service) {
	selfService = service;
}

void LinkLocalRoster::handleStopped(bool error) {
	std::cout << "DNSSDService stopped: " << error << std::endl;
}

bool LinkLocalRoster::hasItem(const JID& j) const {
	for(ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		if (getJIDForService(i->first) == j) {
			return true;
		}
	}
	return false;
}

String LinkLocalRoster::getHostname(const JID& j) const {
	for(ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		if (getJIDForService(i->first) == j) {
			return i->second.host;
		}
	}
	return "";
}

int LinkLocalRoster::getPort(const JID& j) const {
	for(ServiceMap::const_iterator i = services.begin(); i != services.end(); ++i) {
		if (getJIDForService(i->first) == j) {
			return i->second.port;
		}
	}
	return 0;
}

}
