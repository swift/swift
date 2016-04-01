/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class RenameRosterItemUIEvent : public UIEvent {
        public:
            RenameRosterItemUIEvent(const JID& jid, const std::string& newName) : jid_(jid), newName_(newName) {}

            const JID& getJID() const {return jid_;}
            const std::string& getNewName() const {return newName_;}

        private:
            JID jid_;
            std::string newName_;
    };
}
