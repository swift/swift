/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>

namespace Swift {
    class FakeDNSSDQuerier;

    class FakeDNSSDRegisterQuery : public DNSSDRegisterQuery, public FakeDNSSDQuery {
        public:
            FakeDNSSDRegisterQuery(const std::string& name, unsigned short port, const ByteArray& info, std::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), name(name), port(port), info(info) {
            }

            void registerService() {
                run();
            }

            void updateServiceInfo(const ByteArray& i) {
                info = i;
            }

            void unregisterService() {
                finish();
            }

            std::string name;
            unsigned short port;
            ByteArray info;
    };
}
