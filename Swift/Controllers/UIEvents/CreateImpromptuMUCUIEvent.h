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

#include <string>
#include <vector>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

class CreateImpromptuMUCUIEvent : public UIEvent {
    public:
        /**
         * @brief CreateImpromptuMUCUIEvent
         * @param jids A vector of JIDs that are invited to the imprompto MUC.
         *              Useful when the event is used to recreate an old impromptu
         *              chat room.
         * @param roomJID The full JID of the impromtu MUC. Useful when the event
         *                  is used to recreate an old impromptu chat room.
         * @param reason
         */
        CreateImpromptuMUCUIEvent(const std::vector<JID>& jids, const JID& roomJID, const std::string reason) : jids_(jids), roomJID_(roomJID), reason_(reason) { }

        std::vector<JID> getJIDs() const { return jids_; }
        JID getRoomJID() const { return roomJID_; }
        std::string getReason() const { return reason_; }

    private:
        std::vector<JID> jids_;
        JID roomJID_;
        std::string reason_;
};

}
