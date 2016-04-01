/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QList>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/QtUI/ChatList/ChatListItem.h>

namespace Swift {
    class ChatListMUCItem : public ChatListItem {
        public:
            enum MUCItemRoles {
                DetailTextRole = Qt::UserRole/*,
                AvatarRole = Qt::UserRole + 1,
                PresenceIconRole = Qt::UserRole + 2,
                StatusShowTypeRole = Qt::UserRole + 3*/
            };
            ChatListMUCItem(const MUCBookmark& bookmark, ChatListGroupItem* parent);
            const MUCBookmark& getBookmark() const;
            QVariant data(int role) const;
        private:
            MUCBookmark bookmark_;
            QList<ChatListItem*> items_;
    };
}
