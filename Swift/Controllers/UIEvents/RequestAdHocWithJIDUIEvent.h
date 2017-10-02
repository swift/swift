/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class RequestAdHocWithJIDUIEvent : public UIEvent {
        public:
            RequestAdHocWithJIDUIEvent(const JID& jid, const std::string& node) : jid_(jid), node_(node) {}
            JID getJID() const { return jid_; }
            std::string getNode() const { return node_; }
        private:
            JID jid_;
            std::string node_;
    };
}
