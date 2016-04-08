/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <mutex>

#include <dns_sd.h>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
    class BonjourQuerier;
    class EventLoop;

    class BonjourQuery :
            public EventOwner,
            public std::enable_shared_from_this<BonjourQuery> {
        public:
            BonjourQuery(std::shared_ptr<BonjourQuerier>, EventLoop* eventLoop);
            virtual ~BonjourQuery();

            void processResult();
            int getSocketID() const;

        protected:
            void run();
            void finish();

        protected:
            EventLoop* eventLoop;
            std::shared_ptr<BonjourQuerier> querier;
            mutable std::mutex sdRefMutex;
            DNSServiceRef sdRef;
    };
}
