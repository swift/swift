/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>

namespace Swift {
    class UIEventStream;
    class JoinMUCWindowFactory {
        public:
            virtual ~JoinMUCWindowFactory() {}

            virtual JoinMUCWindow* createJoinMUCWindow(UIEventStream* uiEventStream) = 0;
    };
}
