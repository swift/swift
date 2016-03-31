/*
 * Copyright (c) 2010 Isode Limited.
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

            virtual boost::shared_ptr<Timer> createTimer(int milliseconds);
            void setTime(int time);

        private:
            friend class DummyTimer;
            int currentTime;
            std::list<boost::shared_ptr<DummyTimer> > timers;
    };
}
