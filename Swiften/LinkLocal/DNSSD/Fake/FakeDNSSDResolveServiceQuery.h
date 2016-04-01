/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
    class FakeDNSSDQuerier;

    class FakeDNSSDResolveServiceQuery : public DNSSDResolveServiceQuery, public FakeDNSSDQuery {
        public:
            FakeDNSSDResolveServiceQuery(const DNSSDServiceID& service, std::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), service(service) {
            }

            void start() {
                run();
            }

            void stop() {
                finish();
            }

            DNSSDServiceID service;
    };
}
