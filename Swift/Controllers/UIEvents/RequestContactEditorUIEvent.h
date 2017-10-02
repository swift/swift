/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class RequestContactEditorUIEvent : public UIEvent {
        public:
            typedef std::shared_ptr<RequestContactEditorUIEvent> ref;

            RequestContactEditorUIEvent(const JID& jid) : jid(jid) {
            }

            const JID& getJID() const {
                return jid;
            }

        private:
            JID jid;
    };
}
