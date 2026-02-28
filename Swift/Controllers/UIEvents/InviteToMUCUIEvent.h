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

#include <memory>
#include <vector>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class InviteToMUCUIEvent : public UIEvent {
        public:
            typedef std::shared_ptr<InviteToMUCUIEvent> ref;

            InviteToMUCUIEvent(const JID& originator, const std::vector<JID>& JIDsToInvite, const std::string& reason) : originator_(originator), invite_(JIDsToInvite), reason_(reason) {
            }

            const JID& getOriginator() const {
                return originator_;
            }

            const std::vector<JID> getInvites() const {
                return invite_;
            }

            const std::string getReason() const {
                return reason_;
            }

        private:
            JID originator_;
            std::vector<JID> invite_;
            std::string reason_;
    };
}
