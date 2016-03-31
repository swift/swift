/*
 * Copyright (c) 2013 Tobias Markmann
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

class RequestChangeBlockStateUIEvent : public UIEvent {
    public:
        enum BlockState {
            Blocked,
            Unblocked
        };

    public:
        RequestChangeBlockStateUIEvent(BlockState newState, const JID& contact) : state_(newState), contact_(contact) {}

        BlockState getBlockState() const {
            return state_;
        }

        JID getContact() const {
            return contact_;
        }
    private:
        BlockState state_;
        JID contact_;
};

}
