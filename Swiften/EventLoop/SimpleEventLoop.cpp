/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/SimpleEventLoop.h>

#include <boost/bind.hpp>

namespace Swift {

SimpleEventLoop::SimpleEventLoop() : isRunning_(true), eventAvailable_(false) {
}

SimpleEventLoop::~SimpleEventLoop() {
}

void SimpleEventLoop::doRun(bool breakAfterEvents) {
    while (isRunning_) {
        {
            std::unique_lock<std::mutex> lock(eventAvailableMutex_);
            while (!eventAvailable_) {
                eventAvailableCondition_.wait(lock);
            }

            eventAvailable_ = false;
        }
        runOnce();
        if (breakAfterEvents) {
            return;
        }
    }
}

void SimpleEventLoop::runOnce() {
    handleNextEvents();
}

void SimpleEventLoop::stop() {
    postEvent(boost::bind(&SimpleEventLoop::doStop, this));
}

void SimpleEventLoop::doStop() {
    isRunning_ = false;
}

void SimpleEventLoop::eventPosted() {
    {
        std::unique_lock<std::mutex> lock(eventAvailableMutex_);
        eventAvailable_ = true;
    }
    eventAvailableCondition_.notify_one();
}


}
