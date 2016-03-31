/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
    class SWIFTEN_API SimpleEventLoop : public EventLoop {
        public:
            SimpleEventLoop();
            virtual ~SimpleEventLoop();

            void run() {
                doRun(false);
            }

            void runUntilEvents() {
                doRun(true);
            }

            void runOnce();

            void stop();

        protected:
            virtual void eventPosted();

        private:
            void doRun(bool breakAfterEvents);
            void doStop();

        private:
            bool isRunning_;

            bool eventAvailable_;
            boost::mutex eventAvailableMutex_;
            boost::condition_variable eventAvailableCondition_;
    };
}
