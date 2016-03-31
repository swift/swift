/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class EventWindow;

    class EventWindowFactory {
        public:
            virtual ~EventWindowFactory() {}
            /**
             * Transfers ownership of result.
             */
            virtual EventWindow* createEventWindow() = 0;

    };
}


