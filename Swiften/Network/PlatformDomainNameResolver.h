/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Atomic.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>

namespace Swift {
    class IDNConverter;
    class EventLoop;

    class SWIFTEN_API PlatformDomainNameResolver : public DomainNameResolver {
        public:
            PlatformDomainNameResolver(IDNConverter* idnConverter, EventLoop* eventLoop);
            virtual ~PlatformDomainNameResolver();

            virtual DomainNameServiceQuery::ref createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain);
            virtual DomainNameAddressQuery::ref createAddressQuery(const std::string& name);

        private:
            void run();
            void addQueryToQueue(PlatformDomainNameQuery::ref);

        private:
            friend class PlatformDomainNameServiceQuery;
            friend class PlatformDomainNameAddressQuery;
            IDNConverter* idnConverter;
            EventLoop* eventLoop;
            Atomic<bool> stopRequested;
            std::thread* thread;
            std::deque<PlatformDomainNameQuery::ref> queue;
            std::mutex queueMutex;
            std::condition_variable queueNonEmpty;
    };
}
