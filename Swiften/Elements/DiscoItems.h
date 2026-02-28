/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    /**
     * Service discovery disco#items from XEP-0030.
     */
    class SWIFTEN_API DiscoItems : public Payload {
        public:
            /**
             * A single result item.
             */
            class Item {
                public:
                    Item(const std::string& name, const JID& jid, const std::string& node="") : name_(name), jid_(jid), node_(node) {
                    }

                    const std::string& getName() const {
                        return name_;
                    }

                    const std::string& getNode() const {
                        return node_;
                    }

                    const JID& getJID() const {
                        return jid_;
                    }

                private:
                    std::string name_;
                    JID jid_;
                    std::string node_;
            };

            DiscoItems() {
            }

            const std::string& getNode() const {
                return node_;
            }

            void setNode(const std::string& node) {
                node_ = node;
            }

            const std::vector<Item>& getItems() const {
                return items_;
            }

            void addItem(const Item& item) {
                items_.push_back(item);
            }

        private:
            std::string node_;
            std::vector<Item> items_;
    };
}
