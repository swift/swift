/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>

namespace Swift {
    class DNSSDResolveServiceQuery {
        public:
            struct Result {
                Result(const std::string& fullName, const std::string& host, unsigned short port, const ByteArray& info) :
                            fullName(fullName), host(host), port(port), info(info) {}
                std::string fullName;
                std::string host;
                unsigned short port;
                ByteArray info;
            };

            virtual ~DNSSDResolveServiceQuery();

            virtual void start() = 0;
            virtual void stop() = 0;

            boost::signals2::signal<void (const boost::optional<Result>&)> onServiceResolved;
    };
}
