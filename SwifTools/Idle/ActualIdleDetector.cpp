/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Idle/ActualIdleDetector.h>

#include <boost/bind.hpp>

#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {

ActualIdleDetector::ActualIdleDetector(IdleQuerier* querier, TimerFactory* timerFactory, int refreshRateMilliseconds) : querier(querier) {
    timer = timerFactory->createTimer(refreshRateMilliseconds);
    timer->onTick.connect(boost::bind(&ActualIdleDetector::handleTimerTick, this));
    timer->start();
}

ActualIdleDetector::~ActualIdleDetector() {
    timer->onTick.disconnect(boost::bind(&ActualIdleDetector::handleTimerTick, this));
    timer->stop();
}

void ActualIdleDetector::handleTimerTick() {
    timer->stop();
    setIdle(querier->getIdleTimeSeconds() >= getIdleTimeSeconds());
    timer->start();
}

}
