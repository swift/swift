/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>

namespace Swift {
    class EventLoop;

    class PlatformDomainNameServiceQuery : public DomainNameServiceQuery, public PlatformDomainNameQuery, public boost::enable_shared_from_this<PlatformDomainNameServiceQuery>, public EventOwner {
        public:
            PlatformDomainNameServiceQuery(const boost::optional<std::string>& serviceName, EventLoop* eventLoop, PlatformDomainNameResolver* resolver);
            virtual ~PlatformDomainNameServiceQuery();

            virtual void run();

        private:
            void runBlocking();
            void emitError();

        private:
            EventLoop* eventLoop;
            std::string service;
            bool serviceValid;
    };
}
