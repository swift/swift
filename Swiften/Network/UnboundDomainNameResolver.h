/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/asio.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/Timer.h>

struct ub_ctx;
struct ub_result;

namespace Swift {
    class EventLoop;
    class IDNConverter;

    class UnboundDomainNameResolver;
    class UnboundQuery;

    class UnboundDomainNameResolver : public DomainNameResolver, public EventOwner, public std::enable_shared_from_this<UnboundDomainNameResolver> {
        public:
            UnboundDomainNameResolver(IDNConverter* idnConverter, std::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop);
            virtual ~UnboundDomainNameResolver();

            virtual std::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain);
            virtual std::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& name);

            static void unbound_callback_wrapper(void* data, int err, ub_result* result);

        private:
            void unbound_callback(std::shared_ptr<UnboundQuery> query, int err, ub_result* result);

            void handleUBSocketReadable(boost::system::error_code);
            void processData();

        private:
            IDNConverter* idnConverter;
            std::shared_ptr<EventOwner> eventOwner;
            std::shared_ptr<boost::asio::io_service> ioService;
            boost::asio::posix::stream_descriptor ubDescriptior;
            EventLoop* eventLoop;
            ub_ctx* ubContext;
    };

}
