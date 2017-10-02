/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QVariant>

#include <qdebug.h>

namespace Swift {
    class ChatListGroupItem;
    class ChatListItem {
        public:
            ChatListItem(ChatListGroupItem* parent) {parent_ = parent;}
            virtual ~ChatListItem() {}

            ChatListGroupItem* parent() {return parent_;}
            virtual QVariant data(int role) const = 0;

        private:
            ChatListGroupItem* parent_;
    };
}
