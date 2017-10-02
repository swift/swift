/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QList>

#include <Swift/QtUI/ChatList/ChatListItem.h>

namespace Swift {
    class ChatListGroupItem : public ChatListItem {
        public:
            ChatListGroupItem(const QString& name, ChatListGroupItem* parent, bool sorted = true) : ChatListItem(parent), name_(name), sorted_(sorted) {}
            virtual ~ChatListGroupItem() {clear();}
            void addItem(ChatListItem* item) {items_.push_back(item); if (sorted_) {qStableSort(items_.begin(), items_.end(), pointerItemLessThan);}}
            void remove(int index) {items_.removeAt(index);}
            int rowCount() {return items_.size();}
            ChatListItem* item(int i) {return items_[i];}
            int row(ChatListItem* item) {return items_.indexOf(item);}
            QVariant data(int role) const {return (role == Qt::DisplayRole) ? name_ : QVariant();}
            void clear() {
                for (auto item : items_) {
                    delete item;
                }
                items_.clear();
            }

        private:
            static bool pointerItemLessThan(const ChatListItem* first, const ChatListItem* second) {
                QString myName = first->data(Qt::DisplayRole).toString().toLower();
                QString theirName = second->data(Qt::DisplayRole).toString().toLower();
                return myName < theirName;
            }

            QString name_;
            QList<ChatListItem*> items_;
            bool sorted_;
    };
}
