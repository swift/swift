/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

namespace Swift {
    class UIEventStream;
    class ChatListWindowFactory {
        public:
            virtual ~ChatListWindowFactory() {}
            virtual ChatListWindow* createChatListWindow(UIEventStream* uiEventStream) = 0;
    };
}
