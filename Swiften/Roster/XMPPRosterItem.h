/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API XMPPRosterItem {
        public:
            XMPPRosterItem(const JID& jid, const std::string& name, const std::vector<std::string>& groups, RosterItemPayload::Subscription subscription, bool isMIXChannel = false) : jid(jid), name(name), groups(groups), subscription(subscription), isMIXChannel_(isMIXChannel) {
            }

            const JID& getJID() const {
                return jid;
            }

            const std::string& getName() const {
                return name;
            }

            void setName(const std::string& name) {
                this->name = name;
            }

            const std::vector<std::string>& getGroups() const {
                return groups;
            }

            void setGroups(const std::vector<std::string>& groups) {
                this->groups = groups;
            }

            RosterItemPayload::Subscription getSubscription() const {
                return subscription;
            }

            void setMIXChannel(bool isMIXChannel) {
                isMIXChannel_ = isMIXChannel;
            }

            bool isMIXChannel() const {
                return isMIXChannel_;
            }

        private:
            JID jid;
            std::string name;
            std::vector<std::string> groups;
            RosterItemPayload::Subscription subscription;
            bool isMIXChannel_;
    };
}
