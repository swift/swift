/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/asio/io_service.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>

namespace Swift {
    class PlatformDomainNameResolver;
    class EventLoop;

    class PlatformDomainNameAddressQuery : public DomainNameAddressQuery, public PlatformDomainNameQuery, public std::enable_shared_from_this<PlatformDomainNameAddressQuery>, public EventOwner {
        public:
            PlatformDomainNameAddressQuery(const boost::optional<std::string>& host, EventLoop* eventLoop, PlatformDomainNameResolver*);
            virtual ~PlatformDomainNameAddressQuery();

            void run();

        private:
            void runBlocking();
            void emitError();

        private:
            boost::asio::io_service ioService;
            std::string hostname;
            bool hostnameValid;
            EventLoop* eventLoop;
    };
}


