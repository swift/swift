/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#ifndef SWIFTEN_CHATWINDOWFACTORY_H
#define SWIFTEN_CHATWINDOWFACTORY_H

#include "Swiften/JID/JID.h"

namespace Swift {
    class ChatWindow;
    class UIEventStream;
    class ChatWindowFactory {
        public:
            virtual ~ChatWindowFactory() {}
            /**
             * Transfers ownership of result.
             */
            virtual ChatWindow* createChatWindow(const JID &contact, UIEventStream* eventStream) = 0;

    };
}
#endif

