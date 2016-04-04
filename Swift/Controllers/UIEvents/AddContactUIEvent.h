/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>
#include <string>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class AddContactUIEvent : public UIEvent {
        public:
            AddContactUIEvent(const JID& jid, const std::string& name, const std::set<std::string>& groups) : jid_(jid), name_(name), groups_(groups) {}

            const std::string& getName() const {
                return name_;
            }

            const JID& getJID() const {
                return jid_;
            }

            const std::set<std::string>& getGroups() const {
                return groups_;
            }

        private:
            JID jid_;
            std::string name_;
            std::set<std::string> groups_;
    };
}
