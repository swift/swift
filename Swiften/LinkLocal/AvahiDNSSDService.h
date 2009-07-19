#pragma once

#include <map>
#include <boost/enable_shared_from_this.hpp>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>


#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/LinkLocal/DNSSDService.h"

namespace Swift {
	class AvahiDNSSDService : public DNSSDService, public EventOwner, public boost::enable_shared_from_this<AvahiDNSSDService> {
		public:
			AvahiDNSSDService();
			~AvahiDNSSDService();

			virtual void start();
			virtual void stop();

			virtual void registerService(const String& name, int port, const LinkLocalServiceInfo&);
			virtual void updateService(const LinkLocalServiceInfo&);
			virtual void unregisterService();

			virtual void startResolvingService(const Service&);
			virtual void stopResolvingService(const Service&);

			virtual void resolveHostname(const String& hostname, int interfaceIndex = 0);
		
		private:
			static void handleServiceDiscoveredGlobal(AvahiServiceBrowser *b, AvahiIfIndex networkInterface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* userdata) {
				static_cast<AvahiDNSSDService*>(userdata)->handleServiceDiscovered(b, networkInterface, protocol, event, name, type, domain, flags);
			}
			void handleServiceDiscovered(AvahiServiceBrowser *b, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AVAHI_GCC_UNUSED AvahiLookupResultFlags flags);

			static void handleServiceResolvedGlobal(AvahiServiceResolver *r, AvahiIfIndex interfaceIndex, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *hostname, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void* userdata) {
				static_cast<AvahiDNSSDService*>(userdata)->handleServiceResolved(r, interfaceIndex, protocol, event, name, type, domain, hostname, address, port, txt, flags);
			}
			void handleServiceResolved(AvahiServiceResolver *r, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *hostname, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags);


		private:
			AvahiClient* client;
			AvahiThreadedPoll* threadedPoll;
			AvahiServiceBrowser* serviceBrowser;
			typedef std::map<Service, AvahiServiceResolver*> ServiceResolverMap;
			ServiceResolverMap serviceResolvers;
			typedef std::map<String, HostAddress> HostnameAddressMap;
			HostnameAddressMap hostnameAddresses;
	};
}
