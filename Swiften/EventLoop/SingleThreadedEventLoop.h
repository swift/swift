/*
 * Copyright (c) 2010 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

#include <Swiften/EventLoop/EventLoop.h>

// DESCRIPTION:
//
// All interaction with Swiften should happen on the same thread, such as the main GUI thread,
// since the library isn't thread-safe.
// For applications that don't have a main loop, such as WPF and MFC applications, we need a
// different approach to process events from Swiften.
//
// The SingleThreadedEventLoop class implements an event loop that can be used from such applications.
//
// USAGE:
//
// Spawn a new thread in the desired framework and call SingleThreadedEventLoop::waitForEvents(). The method
// blocks until a new event has arrived at which time it'll return, or until the wait is canceled
// at which time it throws EventLoopCanceledException.
//
// When a new event has arrived and SingleThreadedEventLoop::waitForEvents() returns, the caller should then
// call SingleThreadedEventLoop::handleEvents() on the main GUI thread. For WPF applications, for instance,
// the Dispatcher class can be used to execute the call on the GUI thread.
//

namespace Swift {
    class SingleThreadedEventLoop : public EventLoop {
        public:
            class EventLoopCanceledException : public std::exception { };

        public:
            SingleThreadedEventLoop();
            virtual ~SingleThreadedEventLoop();

            // Blocks while waiting for new events and returns when new events are available.
            // Throws EventLoopCanceledException when the wait is canceled.
            void waitForEvents();
            void handleEvents();
            void stop();

        protected:
            virtual void eventPosted();

        private:
            bool shouldShutDown_;

            bool eventAvailable_;
            std::mutex eventAvailableMutex_;
            std::condition_variable eventAvailableCondition_;
    };
}
