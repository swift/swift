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

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class ShowWhiteboardUIEvent : public UIEvent {
    public:
        ShowWhiteboardUIEvent(const JID& contact) : contact_(contact) {}
        const JID& getContact() const {return contact_;}
    private:
        JID contact_;
    };
}

