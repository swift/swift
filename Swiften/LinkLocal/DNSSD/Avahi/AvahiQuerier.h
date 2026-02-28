/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/watch.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>

namespace Swift {
    class EventLoop;

    class AvahiQuerier :
            public DNSSDQuerier,
            public std::enable_shared_from_this<AvahiQuerier> {
        public:
            AvahiQuerier(EventLoop* eventLoop);
            ~AvahiQuerier();

            std::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
            std::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
                    const std::string& name, unsigned short port, const ByteArray& info);
            std::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
                    const DNSSDServiceID&);
            std::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
                    const std::string& hostname, int interfaceIndex);

            void start();
            void stop();

            AvahiThreadedPoll* getThreadedPoll() const {
                return threadedPoll;
            }

            AvahiClient* getClient() const {
                return client;
            }

        private:
            EventLoop* eventLoop;
            AvahiClient* client;
            AvahiThreadedPoll* threadedPoll;
    };
}
