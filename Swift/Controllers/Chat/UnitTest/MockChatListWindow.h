/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

namespace Swift {

    class MockChatListWindow : public ChatListWindow {
        public:
            MockChatListWindow() {}
            virtual ~MockChatListWindow() {}
            void addMUCBookmark(const MUCBookmark& /*bookmark*/) {}
            void removeMUCBookmark(const MUCBookmark& /*bookmark*/) {}
            void addWhiteboardSession(const ChatListWindow::Chat& /*chat*/) {}
            void removeWhiteboardSession(const JID& /*jid*/) {}
            void setBookmarksEnabled(bool /*enabled*/) {}
            void setRecents(const std::list<ChatListWindow::Chat>& /*recents*/) {}
            void setUnreadCount(size_t /*unread*/) {}
            void clearBookmarks() {}
            void setOnline(bool /*isOnline*/) {}
    };

}
