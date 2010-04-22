/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/bind.hpp>

#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class AvahiQuerier;

	class AvahiBrowseQuery : public DNSSDBrowseQuery, public AvahiQuery {
		public:	
			AvahiBrowseQuery(boost::shared_ptr<AvahiQuerier> q) : AvahiQuery(q) {
			}

			void startBrowsing() {
				std::cout << "Start browsing" << std::endl;
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				std::cout << "Creating browser" << std::endl;
				AvahiServiceBrowser* browser = avahi_service_browser_new(querier->getClient(), AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_presence._tcp", NULL, (AvahiLookupFlags) 0, &handleServiceDiscoveredStatic, this);
				if (!browser) {
					std::cout << "Error" << std::endl;
					MainEventLoop::postEvent(boost::bind(boost::ref(onError)), shared_from_this());
				}
				std::cout << "Unlocking" << std::endl;
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
				std::cout << "Browse started" << std::endl;
			}

			void stopBrowsing() {
				assert(browser);
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				avahi_server_browser_free(browser);
				browser = NULL;
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
			}

		private:
			static void handleServiceDiscoveredStatic(AvahiServiceBrowser *b, AvahiIfIndex interfaceIndex, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* context) {
				static_cast<AvahiBrowseQuery*>(context)->handleServiceDiscovered(b, interfaceIndex, protocol, event, name, type, domain, flags);
			}

			void handleServiceDiscovered(AvahiServiceBrowser *, AvahiIfIndex interfaceIndex, AvahiProtocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags) {
				switch (event) {
					case AVAHI_BROWSER_FAILURE:	
						std::cout << "Service browse error" << std::endl;
						MainEventLoop::postEvent(boost::bind(boost::ref(onError)), shared_from_this());
						break;
					case AVAHI_BROWSER_NEW: {
						DNSSDServiceID service(name, domain, type, interfaceIndex);
						std::cout << "Service discovered " << name << " " << type << " " << domain << std::endl;
						MainEventLoop::postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
						break;
					}
					case AVAHI_BROWSER_REMOVE: {
						std::cout << "Service went away " << name << " " << type << " " << domain << std::endl;
						DNSSDServiceID service(name, domain, type, interfaceIndex);
						MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
						break;
					}
					case AVAHI_BROWSER_ALL_FOR_NOW:
					case AVAHI_BROWSER_CACHE_EXHAUSTED:
						break;
				}
			}
	};
}
