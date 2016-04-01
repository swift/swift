/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <SwifTools/Idle/IdleDetector.h>

namespace Swift {
    class IdleQuerier;
    class TimerFactory;
    class Timer;

    class ActualIdleDetector : public IdleDetector, public boost::bsignals::trackable {
        public:
            ActualIdleDetector(IdleQuerier*, TimerFactory*, int refreshRateMilliseconds);
            ~ActualIdleDetector();

        private:
            void handleTimerTick();

        private:
            IdleQuerier* querier;
            std::shared_ptr<Timer> timer;
    };
}
