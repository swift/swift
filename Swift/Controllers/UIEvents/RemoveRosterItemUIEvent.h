/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

class RemoveRosterItemUIEvent : public UIEvent {
    public:
        RemoveRosterItemUIEvent(const JID& jid) : jid_(jid) {}
        virtual ~RemoveRosterItemUIEvent() {}
        JID getJID() {return jid_;}
    private:
        JID jid_;

};

}
