/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <avahi-client/lookup.h>

#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>

namespace Swift {
    class AvahiQuerier;

    class AvahiBrowseQuery : public DNSSDBrowseQuery, public AvahiQuery {
        public:
            AvahiBrowseQuery(std::shared_ptr<AvahiQuerier> q, EventLoop* eventLoop) : AvahiQuery(q, eventLoop), browser(NULL) {
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
