/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <mutex>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
    class SWIFTEN_API DummyEventLoop : public EventLoop {
        public:
            DummyEventLoop();
            virtual ~DummyEventLoop();

            void processEvents();

            bool hasEvents();

            virtual void eventPosted();

        private:
            bool hasEvents_;
            std::mutex hasEventsMutex_;
    };
}
