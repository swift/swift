/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Elements/Presence.h>

namespace Swift {

    class StatusTracker {
        public:
            StatusTracker();
            std::shared_ptr<Presence> getNextPresence();
            void setRequestedPresence(std::shared_ptr<Presence> presence);
            bool goAutoAway(const int& seconds);
            bool goAutoUnAway();
        private:
            std::shared_ptr<Presence> queuedPresence_;
            bool isAutoAway_;
            boost::posix_time::ptime isAutoAwaySince_;
    };
}
