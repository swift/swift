/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
    class FakeDNSSDQuerier;

    class FakeDNSSDQuery :
            public EventOwner,
            public std::enable_shared_from_this<FakeDNSSDQuery> {
        public:
            FakeDNSSDQuery(std::shared_ptr<FakeDNSSDQuerier>);
            virtual ~FakeDNSSDQuery();

        protected:
            void run();
            void finish();

        protected:
            std::shared_ptr<FakeDNSSDQuerier> querier;
    };
}
