/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    typedef std::vector<std::shared_ptr<StanzaEvent> > EventList;
    class EventController {
        public:
            EventController();
            ~EventController();

            void handleIncomingEvent(std::shared_ptr<StanzaEvent> sourceEvent);
            boost::signals2::signal<void (size_t)> onEventQueueLengthChange;
            boost::signals2::signal<void (std::shared_ptr<StanzaEvent>)> onEventQueueEventAdded;
            const EventList& getEvents() const {return events_;}
            void disconnectAll();
            void clear();

        private:
            void handleEventConcluded(std::shared_ptr<StanzaEvent> event);
            EventList events_;
    };
}
