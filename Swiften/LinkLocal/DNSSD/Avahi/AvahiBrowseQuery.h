/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <avahi-client/lookup.h>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class AvahiQuerier;

	class AvahiBrowseQuery : public DNSSDBrowseQuery, public AvahiQuery {
		public:	
			AvahiBrowseQuery(boost::shared_ptr<AvahiQuerier> q, EventLoop* eventLoop) : AvahiQuery(q, eventLoop), browser(NULL) {
			}

			void startBrowsing();
			void stopBrowsing();

		private:
			static void handleServiceDiscoveredStatic(AvahiServiceBrowser *b, AvahiIfIndex interfaceIndex, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* context) {
				static_cast<AvahiBrowseQuery*>(context)->handleServiceDiscovered(b, interfaceIndex, protocol, event, name, type, domain, flags);
			}

			void handleServiceDiscovered(AvahiServiceBrowser *, AvahiIfIndex interfaceIndex, AvahiProtocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags);

		private:
			AvahiServiceBrowser* browser;
			
	};
}
