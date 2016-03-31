/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <netinet/in.h>

#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class AvahiQuerier;

    class AvahiResolveHostnameQuery : public DNSSDResolveHostnameQuery, public AvahiQuery {
        public:
            AvahiResolveHostnameQuery(const std::string& hostname, int, boost::shared_ptr<AvahiQuerier> querier, EventLoop* eventLoop);

            void run();

            void finish() {
            }

        private:
            HostAddress hostAddress;
            std::string hostname;
    };
}
