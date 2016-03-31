/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Client/BlockList.h>

namespace Swift {
    class BlockListImpl : public BlockList {
        public:
            BlockListImpl();

            virtual State getState() const {
                return state;
            }

            void setState(State state);

            virtual const std::vector<JID>& getItems() const {
                return items;
            }

            void setItems(const std::vector<JID>& newItems);
            void addItem(const JID& item);
            void removeItem(const JID& item);
            void addItems(const std::vector<JID>& items);
            void removeItems(const std::vector<JID>& items);
            void removeAllItems();

        private:
            State state;
            std::vector<JID> items;
    };
}
