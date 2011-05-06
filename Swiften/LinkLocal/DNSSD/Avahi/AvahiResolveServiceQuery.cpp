/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveServiceQuery.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>

namespace Swift {

void AvahiResolveServiceQuery::start() {
	std::cout << "Start resolving " << service.getName() << " " << service.getType() << " " << service.getDomain() << std::endl;
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	assert(!resolver);
	resolver = avahi_service_resolver_new(querier->getClient(), service.getNetworkInterfaceID(), AVAHI_PROTO_UNSPEC, service.getName().c_str(), service.getType().c_str(), service.getDomain().c_str(), AVAHI_PROTO_UNSPEC, static_cast<AvahiLookupFlags>(0), handleServiceResolvedStatic, this);
	if (!resolver) {
		std::cout << "Error starting resolver" << std::endl;
		eventLoop->postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
	}
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiResolveServiceQuery::stop() {
	std::cout << "Stop resolving" << std::endl;
	avahi_threaded_poll_lock(querier->getThreadedPoll());
	avahi_service_resolver_free(resolver);
	resolver = NULL;
	avahi_threaded_poll_unlock(querier->getThreadedPoll());
}

void AvahiResolveServiceQuery::handleServiceResolved(AvahiServiceResolver* resolver, AvahiIfIndex, AvahiProtocol, AvahiResolverEvent event, const char *name, const char * type, const char* domain, const char * /*host_name*/, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags) {
	std::cout << "Resolve finished" << std::endl;
	switch(event) {
		case AVAHI_RESOLVER_FAILURE:
			std::cout << "Resolve error " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(resolver))) << std::endl;
			eventLoop->postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
			break;
		case AVAHI_RESOLVER_FOUND: {
			std::cout << "Success" << std::endl;
			char a[AVAHI_ADDRESS_STR_MAX];
			avahi_address_snprint(a, sizeof(a), address);

			ByteArray txtRecord;
			txtRecord.resize(1024);
			avahi_string_list_serialize(txt, vecptr(txtRecord), txtRecord.size());

			// FIXME: Probably not accurate
			std::string fullname = std::string(name) + "." + std::string(type) + "." + std::string(domain) + ".";
			std::cout << "Result: " << fullname << "->" << std::string(a) << ":" << port << std::endl;
			eventLoop->postEvent(
					boost::bind(
						boost::ref(onServiceResolved), 
						Result(fullname, std::string(a), port, txtRecord)),
					shared_from_this());
			break;
		}
	}
}

}
