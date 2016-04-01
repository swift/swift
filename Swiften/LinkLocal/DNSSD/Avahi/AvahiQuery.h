/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
    class AvahiQuerier;
    class EventLoop;

    class AvahiQuery :
            public EventOwner,
            public std::enable_shared_from_this<AvahiQuery> {
        public:
            AvahiQuery(std::shared_ptr<AvahiQuerier>, EventLoop* eventLoop);
            virtual ~AvahiQuery();

        protected:
            std::shared_ptr<AvahiQuerier> querier;
            EventLoop* eventLoop;
    };
}

