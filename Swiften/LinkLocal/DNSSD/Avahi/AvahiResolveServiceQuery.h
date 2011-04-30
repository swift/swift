/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <avahi-client/lookup.h>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class AvahiQuerier;

	class AvahiResolveServiceQuery : public DNSSDResolveServiceQuery, public AvahiQuery {
		public:	
			AvahiResolveServiceQuery(const DNSSDServiceID& service, boost::shared_ptr<AvahiQuerier> querier, EventLoop* eventLoop) : AvahiQuery(querier, eventLoop), service(service), resolver(NULL) {
			}

			void start();
			void stop();

		private:
			static void handleServiceResolvedStatic(AvahiServiceResolver* resolver, AvahiIfIndex interfaceIndex, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void* context) {
				static_cast<AvahiResolveServiceQuery*>(context)->handleServiceResolved(resolver, interfaceIndex, protocol, event, name, type, domain, host_name, address, port, txt, flags);
			}

			void handleServiceResolved(AvahiServiceResolver* resolver, AvahiIfIndex, AvahiProtocol, AvahiResolverEvent event, const char *name, const char * type, const char* domain, const char * /*host_name*/, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags);

		private:
			DNSSDServiceID service;
			AvahiServiceResolver* resolver;
	};
}
