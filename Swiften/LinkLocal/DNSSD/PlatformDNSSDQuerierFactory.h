/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class DNSSDQuerier;
    class EventLoop;

    class SWIFTEN_API PlatformDNSSDQuerierFactory {
        public:
            PlatformDNSSDQuerierFactory(EventLoop* eventLoop);

            std::shared_ptr<DNSSDQuerier> createQuerier();

            bool canCreate();

        private:
            EventLoop* eventLoop;
    };
}
