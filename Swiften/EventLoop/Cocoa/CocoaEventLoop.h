/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <mutex>

#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
    class CocoaEventLoop : public EventLoop {
        public:
            CocoaEventLoop();
            virtual ~CocoaEventLoop();

            void handleNextCocoaEvent();

        protected:
            virtual void eventPosted();

        private:
            bool isEventInCocoaEventLoop_;
            std::recursive_mutex isEventInCocoaEventLoopMutex_;
    };
}
