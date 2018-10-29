/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class DNSSDServiceID;
    class DNSSDBrowseQuery;
    class DNSSDRegisterQuery;
    class DNSSDResolveServiceQuery;
    class DNSSDResolveHostnameQuery;

    class DNSSDQuerier {
        public:
            virtual ~DNSSDQuerier();

            virtual void start() = 0;
            virtual void stop() = 0;

            virtual std::shared_ptr<DNSSDBrowseQuery> createBrowseQuery() = 0;
            virtual std::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
                    const std::string& name, unsigned short port, const ByteArray& info) = 0;
            virtual std::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
                    const DNSSDServiceID&) = 0;
            virtual std::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
                    const std::string& hostname, int interfaceIndex) = 0;
    };
}
