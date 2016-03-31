/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/Timer.h>

struct ub_ctx;
struct ub_result;

namespace Swift {
    class EventLoop;
    class IDNConverter;
    class TimerFactory;

    class UnboundDomainNameResolver;
    class UnboundQuery;

    class UnboundDomainNameResolver : public DomainNameResolver, public EventOwner, public boost::enable_shared_from_this<UnboundDomainNameResolver> {
        public:
            UnboundDomainNameResolver(IDNConverter* idnConverter, boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop);
            virtual ~UnboundDomainNameResolver();

            virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain);
            virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& name);

            static void unbound_callback_wrapper(void* data, int err, ub_result* result);

        private:
            void unbound_callback(boost::shared_ptr<UnboundQuery> query, int err, ub_result* result);

            void handleUBSocketReadable(boost::system::error_code);
            void processData();

        private:
            IDNConverter* idnConverter;
            boost::shared_ptr<EventOwner> eventOwner;
            boost::shared_ptr<boost::asio::io_service> ioService;
            boost::asio::posix::stream_descriptor ubDescriptior;
            EventLoop* eventLoop;
            ub_ctx* ubContext;
    };

}
