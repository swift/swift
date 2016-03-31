/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <set>

#include <Swiften/Base/API.h>
#include <Swiften/Roster/XMPPRoster.h>

namespace Swift {
    class SWIFTEN_API XMPPRosterImpl : public XMPPRoster {
        public:
            XMPPRosterImpl();
            virtual ~XMPPRosterImpl();

            void addContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, RosterItemPayload::Subscription subscription);
            void removeContact(const JID& jid);
            void clear();

            bool containsJID(const JID& jid);
            RosterItemPayload::Subscription getSubscriptionStateForJID(const JID& jid);
            std::string getNameForJID(const JID& jid) const;
            std::vector<std::string> getGroupsForJID(const JID& jid);

            virtual std::vector<XMPPRosterItem> getItems() const;
            virtual boost::optional<XMPPRosterItem> getItem(const JID&) const;
            virtual std::set<std::string> getGroups() const;

        private:
            typedef std::map<JID, XMPPRosterItem> RosterMap;
            RosterMap entries_;
    };
}
