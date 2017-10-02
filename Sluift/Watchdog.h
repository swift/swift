/*
 * Copyright (c) 2011-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <algorithm>

#include <Swiften/Network/TimerFactory.h>

namespace Swift {
    class Watchdog {
        public:
            Watchdog(int timeout, TimerFactory* timerFactory);
            ~Watchdog();

            bool getTimedOut() const {
                return timedOut;
            }

        private:
            void handleTimerTick();

        private:
            Timer::ref timer;
            int remainingTime;
            TimerFactory* timerFactory;
            bool timedOut;
    };
}
