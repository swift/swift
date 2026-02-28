/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class FakeDNSSDQuerier;

    class FakeDNSSDResolveHostnameQuery : public DNSSDResolveHostnameQuery, public FakeDNSSDQuery {
        public:
            FakeDNSSDResolveHostnameQuery(const std::string& hostname, int interfaceIndex, std::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), hostname(hostname), interfaceIndex(interfaceIndex) {
            }

            void run() {
                FakeDNSSDQuery::run();
            }

            void finish() {
                FakeDNSSDQuery::finish();
            }

            std::string hostname;
            int interfaceIndex;
    };
}
