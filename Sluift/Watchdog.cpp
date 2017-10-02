/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Watchdog.h>

#include <Sluift/globals.h>

static const int INTERVAL_MS = 500;


using namespace Swift;

Watchdog::Watchdog(int timeout, TimerFactory* timerFactory) :
        remainingTime(timeout),
        timerFactory(timerFactory),
        timedOut(false) {
    Sluift::globals.interruptRequested = 0;

    int nextTimeout = remainingTime >= 0 ? std::min(remainingTime, INTERVAL_MS) : INTERVAL_MS;

    timer = timerFactory->createTimer(nextTimeout);
    timer->onTick.connect(boost::bind(&Watchdog::handleTimerTick, this));
    remainingTime -= nextTimeout;
    timer->start();
}

Watchdog::~Watchdog() {
    if (timer) {
        timer->stop();
    }
}

void Watchdog::handleTimerTick() {
    if (Sluift::globals.interruptRequested || remainingTime == 0) {
        timedOut = true;
    }
    else {
        int nextTimeout = remainingTime >= 0 ? std::min(remainingTime, INTERVAL_MS) : INTERVAL_MS;
        if (nextTimeout != INTERVAL_MS) {
            timer->onTick.disconnect(boost::bind(&Watchdog::handleTimerTick, this));
            timer = timerFactory->createTimer(nextTimeout);
            timer->onTick.connect(boost::bind(&Watchdog::handleTimerTick, this));
        }
        remainingTime -= nextTimeout;
        timer->start();
    }
}
