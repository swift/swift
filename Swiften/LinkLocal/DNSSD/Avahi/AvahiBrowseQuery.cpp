/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiBrowseQuery.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>

namespace Swift {

void AvahiBrowseQuery::startBrowsing() {
	std::cout << "Start browsing" << std::endl;
	assert(!browser);
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	browser = avahi_service_browser_new(querier->getClient(), AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_presence._tcp", NULL, static_cast<AvahiLookupFlags>(0), &handleServiceDiscoveredStatic, this);
	if (!browser) {
		std::cout << "Error" << std::endl;
		eventLoop->postEvent(boost::bind(boost::ref(onError)), shared_from_this());
	}
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiBrowseQuery::stopBrowsing() {
	std::cout << "Stop browsing" << std::endl;
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	avahi_service_browser_free(browser);
	browser = NULL;
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiBrowseQuery::handleServiceDiscovered(AvahiServiceBrowser *, AvahiIfIndex interfaceIndex, AvahiProtocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags) {
	switch (event) {
		case AVAHI_BROWSER_FAILURE:	
			std::cout << "Service browse error" << std::endl;
			eventLoop->postEvent(boost::bind(boost::ref(onError)), shared_from_this());
			break;
		case AVAHI_BROWSER_NEW: {
			DNSSDServiceID service(name, domain, type, interfaceIndex);
			std::cout << "Service discovered " << name << " " << domain << " " << type << " " << interfaceIndex << std::endl;
			eventLoop->postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
			break;
		}
		case AVAHI_BROWSER_REMOVE: {
			std::cout << "Service went away " << name << " " << domain << " " << type << " " << interfaceIndex << std::endl;
			DNSSDServiceID service(name, domain, type, interfaceIndex);
			eventLoop->postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
			break;
		}
		case AVAHI_BROWSER_ALL_FOR_NOW:
		case AVAHI_BROWSER_CACHE_EXHAUSTED:
			break;
	}
}

}
