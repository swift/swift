/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class EventWindow {
        public:
            EventWindow(bool candelete = true) : canDelete_(candelete) {}

            bool canDelete() const {
                return canDelete_;
            }

            virtual ~EventWindow() {}
            virtual void addEvent(boost::shared_ptr<StanzaEvent> event, bool active) = 0;
            virtual void removeEvent(boost::shared_ptr<StanzaEvent> event) = 0;

        private:
            bool canDelete_;
    };
}
