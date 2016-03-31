/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
    class AvahiQuerier;
    class EventLoop;

    class AvahiQuery :
            public EventOwner,
            public boost::enable_shared_from_this<AvahiQuery> {
        public:
            AvahiQuery(boost::shared_ptr<AvahiQuerier>, EventLoop* eventLoop);
            virtual ~AvahiQuery();

        protected:
            boost::shared_ptr<AvahiQuerier> querier;
            EventLoop* eventLoop;
    };
}

