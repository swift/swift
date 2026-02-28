/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QVariant>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class QtEvent {
        public:
            QtEvent(std::shared_ptr<StanzaEvent> event, bool active);
            QVariant data(int role);
            std::shared_ptr<StanzaEvent> getEvent() { return event_; }
            enum EventRoles {
                SenderRole = Qt::UserRole

            };

        private:
            QString text();
            QString sender();
            std::shared_ptr<StanzaEvent> event_;
            bool active_;
    };
}
