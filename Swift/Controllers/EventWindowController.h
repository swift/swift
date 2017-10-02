/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/EventWindow.h>
#include <Swift/Controllers/UIInterfaces/EventWindowFactory.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

namespace Swift {

    class EventWindowController {
        public:
            EventWindowController(EventController* eventController, EventWindowFactory* windowFactory);
            ~EventWindowController();
        private:
            void handleEventQueueEventAdded(std::shared_ptr<StanzaEvent> event);
            void handleEventConcluded(std::shared_ptr<StanzaEvent> event);

            EventController* eventController_;
            EventWindowFactory* windowFactory_;
            EventWindow* window_;
            boost::signals2::scoped_connection eventAddedConnection_;
    };

}
