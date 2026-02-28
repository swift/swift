/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QIcon>
#include <QList>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

#include <Swift/QtUI/ChatList/ChatListItem.h>

namespace Swift {
    class ChatListWhiteboardItem : public ChatListItem {
        public:
            enum RecentItemRoles {
                DetailTextRole = Qt::UserRole,
                AvatarRole = Qt::UserRole + 1,
                PresenceIconRole = Qt::UserRole + 2/*,
                StatusShowTypeRole = Qt::UserRole + 3*/
            };
            ChatListWhiteboardItem(const ChatListWindow::Chat& chat, ChatListGroupItem* parent);
            const ChatListWindow::Chat& getChat() const;
            QVariant data(int role) const;
        private:
            QIcon getPresenceIcon() const;
            ChatListWindow::Chat chat_;
    };
}
