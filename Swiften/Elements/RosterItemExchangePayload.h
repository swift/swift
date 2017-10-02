/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API RosterItemExchangePayload : public Payload {
        public:
            typedef std::shared_ptr<RosterItemExchangePayload> ref;

            class SWIFTEN_API Item {
                public:
                    enum Action { Add, Modify, Delete };

                    Item(Action action = Add);

                    Action getAction() const {
                        return action;
                    }

                    void setAction(Action action) {
                        this->action = action;
                    }

                    const JID& getJID() const {
                        return jid;
                    }

                    void setJID(const JID& jid) {
                        this->jid = jid;
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

                    void setGroups(const std::vector<std::string> &groups) {
                        this->groups = groups;
                    }

                    void addGroup(const std::string& group) {
                        groups.push_back(group);
                    }

                private:
                    Action action;
                    JID jid;
                    std::string name;
                    std::vector<std::string> groups;
            };

            typedef std::vector<RosterItemExchangePayload::Item> RosterItemExchangePayloadItems;

        public:
            RosterItemExchangePayload();

            void addItem(const RosterItemExchangePayload::Item& item) {
                items_.push_back(item);
            }

            const RosterItemExchangePayloadItems& getItems() const {
                return items_;
            }

        private:
            RosterItemExchangePayloadItems items_;
    };
}
