/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>

namespace Swift {

    class HighlightManager;
    class NickResolver;

    class Highlighter {
        public:
            Highlighter(HighlightManager* manager, NickResolver* nickResolver);

            void handleSystemNotifications(const ChatWindow::ChatMessage& message, std::shared_ptr<MessageEvent> event);
            void handleSoundNotifications(const ChatWindow::ChatMessage& chatMessage);

        private:
            HighlightManager* manager_;
            NickResolver* nickResolver_;
    };

}
