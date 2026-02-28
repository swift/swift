/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/StatusShow.h>

namespace Swift {
    class EventController;
    class SystemTray;

    class SystemTrayController {
        public:
            SystemTrayController(EventController* eventController, SystemTray* systemTray);
            void setMyStatusType(StatusShow::Type type);
            void setConnecting();
        private:
            void handleEventQueueLengthChange(int length);

        private:
            EventController* eventController_;
            SystemTray* systemTray_;
    };
}
