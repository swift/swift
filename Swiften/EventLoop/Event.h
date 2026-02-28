/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/function.hpp>

#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
    class Event {
        public:
            Event(std::shared_ptr<EventOwner> owner, const boost::function<void()>& callback) : id(~0U), owner(owner), callback(callback) {
            }

            bool operator==(const Event& o) const {
                return o.id == id;
            }

            unsigned int id;
            std::shared_ptr<EventOwner> owner;
            boost::function<void()> callback;
    };
}

std::ostream& operator<<(std::ostream& os, const Swift::Event& e);
