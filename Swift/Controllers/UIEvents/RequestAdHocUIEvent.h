/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>

namespace Swift {
    class RequestAdHocUIEvent : public UIEvent {
        public:
            RequestAdHocUIEvent(const DiscoItems::Item& command) : command_(command) {}
            const DiscoItems::Item& getCommand() const {return command_;}
        private:
            DiscoItems::Item command_;
    };
}
