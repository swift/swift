/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class CancelWhiteboardSessionUIEvent : public UIEvent {
        typedef std::shared_ptr<CancelWhiteboardSessionUIEvent> ref;
    public:
        CancelWhiteboardSessionUIEvent(const JID& jid) : jid_(jid) {}
        const JID& getContact() const {return jid_;}
    private:
        JID jid_;
    };
}
