/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class RequestInviteToMUCUIEvent : public UIEvent {
        public:
            typedef std::shared_ptr<RequestInviteToMUCUIEvent> ref;

            enum ImpromptuMode {
                Impromptu,
                NotImpromptu
            };

            /**
             * @brief RequestInviteToMUCUIEvent
             * @param originator This can be a MUC JID if the user wants to invite
             *          people to an existing MUC, or a contact JID if this is the
             *          start of an impromptu group chat.
             * @param JIDsToInvite This is a std::vector of JIDs which are prefilled
             *          in the invite dialog.
             * @param impromptu This flag indicates whether it is a normal MUC invite
             *          or an impromptu MUC invite.
             */
            RequestInviteToMUCUIEvent(const JID& originator, const std::vector<JID>& JIDsToInvite, ImpromptuMode impromptu) : originator_(originator), invite_(JIDsToInvite) {
                isImpromptu_ = impromptu == Impromptu;
            }

            const JID& getOriginator() const {
                return originator_;
            }

            const std::vector<JID> getInvites() const {
                return invite_;
            }

            bool isImpromptu() const {
                return isImpromptu_;
            }

        private:
            JID originator_;
            std::vector<JID> invite_;
            bool isImpromptu_;
    };
}
