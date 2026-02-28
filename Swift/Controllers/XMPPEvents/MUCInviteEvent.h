/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {

    class MUCInviteEvent : public StanzaEvent {
    public:
        typedef std::shared_ptr<MUCInviteEvent> ref;

    public:
        MUCInviteEvent(const JID& inviter, const JID& roomJID, const std::string& reason, const std::string& password, bool direct, bool impromptu) : inviter_(inviter), roomJID_(roomJID), reason_(reason), password_(password), direct_(direct), impromptu_(impromptu) {}

        const JID& getInviter() const { return inviter_; }
        const JID& getRoomJID() const { return roomJID_; }
        const std::string& getReason() const { return reason_; }
        const std::string& getPassword() const { return password_; }
        bool getDirect() const { return direct_; }
        bool getImpromptu() const { return impromptu_; }

    private:
        JID inviter_;
        JID roomJID_;
        std::string reason_;
        std::string password_;
        bool direct_;
        bool impromptu_;
    };
}
