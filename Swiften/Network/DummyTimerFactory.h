/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>

#include <Swiften/Base/API.h>
#include <Swiften/Network/TimerFactory.h>

namespace Swift {
    class SWIFTEN_API DummyTimerFactory : public TimerFactory {
        public:
            class DummyTimer;

            DummyTimerFactory();

            virtual std::shared_ptr<Timer> createTimer(int milliseconds);
            void setTime(int time);

        private:
            friend class DummyTimer;
            int currentTime;
            std::list<std::shared_ptr<DummyTimer> > timers;
    };
}
