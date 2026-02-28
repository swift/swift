/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindow.h>

namespace Swift {
class AdHocCommandWindow;
    class AdHocCommandWindowFactory {
        public:
            virtual ~AdHocCommandWindowFactory() {}
            virtual AdHocCommandWindow* createAdHocCommandWindow(std::shared_ptr<OutgoingAdHocCommandSession> command) = 0;
    };
}
