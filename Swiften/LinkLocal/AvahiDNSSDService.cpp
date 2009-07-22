#include "Swiften/LinkLocal/AvahiDNSSDService.h"

#include <boost/bind.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {

AvahiDNSSDService::AvahiDNSSDService() : client(NULL), threadedPoll(0), serviceBrowser(0) {
}

AvahiDNSSDService::~AvahiDNSSDService() {
}

void AvahiDNSSDService::start() {
	threadedPoll = avahi_threaded_poll_new();

	int error;
	client = avahi_client_new(
			avahi_threaded_poll_get(threadedPoll), 
			static_cast<AvahiClientFlags>(0), NULL, this, &error); // TODO
	if (!client) {
		// TODO
		std::cerr << "Avahi Error: " << avahi_strerror(error) << std::endl;
		return;
	}

	serviceBrowser = avahi_service_browser_new(
			client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 
			"_presence._tcp", NULL, static_cast<AvahiLookupFlags>(0), 
			handleServiceDiscoveredGlobal, this);
	if (!serviceBrowser) {
		// TODO
		std::cerr << "Avahi Error: " << avahi_strerror(error) << std::endl;
		return;
	}

	avahi_threaded_poll_start(threadedPoll);
}

void AvahiDNSSDService::stop() {
	avahi_threaded_poll_stop(threadedPoll);
	avahi_service_browser_free(serviceBrowser);
	avahi_client_free(client);
	avahi_threaded_poll_free(threadedPoll);
}

void AvahiDNSSDService::registerService(const String& name, int port, const LinkLocalServiceInfo& info) {
	avahi_threaded_poll_lock(threadedPoll);
	avahi_threaded_poll_unlock(threadedPoll);
}

void AvahiDNSSDService::updateService(const LinkLocalServiceInfo& info) {
	avahi_threaded_poll_lock(threadedPoll);
	avahi_threaded_poll_unlock(threadedPoll);
}

void AvahiDNSSDService::unregisterService() {
	avahi_threaded_poll_lock(threadedPoll);
	avahi_threaded_poll_unlock(threadedPoll);
}

void AvahiDNSSDService::startResolvingService(const LinkLocalServiceID& service) {
	avahi_threaded_poll_lock(threadedPoll);

	AvahiServiceResolver* resolver = avahi_service_resolver_new(
			client, 
			service.getNetworkInterfaceID(), 
			AVAHI_PROTO_INET,
			service.getName().getUTF8Data(), 
			service.getType().getUTF8Data(), 
			service.getDomain().getUTF8Data(), 
			AVAHI_PROTO_UNSPEC, 
			static_cast<AvahiLookupFlags>(0), 
			&AvahiDNSSDService::handleServiceResolvedGlobal, 
			this);
	assert(serviceResolvers.find(service) == serviceResolvers.end());
	serviceResolvers[service] = resolver;

	avahi_threaded_poll_unlock(threadedPoll);
}

void AvahiDNSSDService::stopResolvingService(const Service& service) {
	avahi_threaded_poll_lock(threadedPoll);

	ServiceResolverMap::iterator i = serviceResolvers.find(service);
	assert(i != serviceResolvers.end());
	avahi_service_resolver_free(i->second);
	serviceResolvers.erase(i);

	avahi_threaded_poll_unlock(threadedPoll);
}

// TODO: Take interfaceIndex into account
void AvahiDNSSDService::resolveHostname(const String& hostname, int) {
	HostnameAddressMap::const_iterator i = hostnameAddresses.find(hostname);
	if (i == hostnameAddresses.end()) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onHostnameResolved), hostname, boost::optional<HostAddress>()), shared_from_this());
	}
	else {
		MainEventLoop::postEvent(boost::bind(boost::ref(onHostnameResolved), hostname, boost::optional<HostAddress>(i->second)), shared_from_this());
	}
}

void AvahiDNSSDService::handleServiceDiscovered(AvahiServiceBrowser *, AvahiIfIndex interfaceIndex, AvahiProtocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags) {
	switch(event) {
		case AVAHI_BROWSER_FAILURE:
			std::cerr << "Browse error" << std::endl;
			// TODO	
			return;
		case AVAHI_BROWSER_NEW: {
				std::cerr << "Service added: " << name << " " << type << " " << domain << std::endl;
				LinkLocalServiceID service(name, type, domain, interfaceIndex);
				MainEventLoop::postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
			}
			break;
		case AVAHI_BROWSER_REMOVE: {
				std::cerr << "Service removed: " << name << " " << type << " " << domain << std::endl;
				LinkLocalServiceID service(name, type, domain, interfaceIndex);
				MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
			}
			break;
		case AVAHI_BROWSER_ALL_FOR_NOW:
		case AVAHI_BROWSER_CACHE_EXHAUSTED:
			break;
	}
}

void AvahiDNSSDService::handleServiceResolved(AvahiServiceResolver *, AvahiIfIndex interfaceIndex, AvahiProtocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *hostname, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags) {
	switch(event) {
		case AVAHI_RESOLVER_FAILURE:
			//TODO
			std::cerr << "Resolve error" << std::endl;
			break;
		case AVAHI_RESOLVER_FOUND:
			ByteArray data;
			for(AvahiStringList* i = txt; i; i = avahi_string_list_get_next(i)) {
				char size = i->size;
				data += ByteArray(&size, 1);
				data += ByteArray(reinterpret_cast<char*>(avahi_string_list_get_text(i)), avahi_string_list_get_size(i));
			}
			
			assert(address->proto == AVAHI_PROTO_INET);
			HostAddress hostAddress(reinterpret_cast<const unsigned char*>(&address->data.ipv4.address), 4);
			hostnameAddresses[String(hostname)] = hostAddress;
			MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), 
					LinkLocalServiceID(name, type, domain, interfaceIndex), 
					ResolveResult(hostname, port, 
						LinkLocalServiceInfo::createFromTXTRecord(data))),
					shared_from_this());
			break;
	}
}

}
