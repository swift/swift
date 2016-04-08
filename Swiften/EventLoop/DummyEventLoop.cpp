/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/DummyEventLoop.h>

#include <iostream>

namespace Swift {

DummyEventLoop::DummyEventLoop() : hasEvents_(false) {
}

DummyEventLoop::~DummyEventLoop() {
    if (hasEvents()) {
        std::cerr << "DummyEventLoop: Unhandled events at destruction time" << std::endl;
    }
}

void DummyEventLoop::processEvents() {
    while(hasEvents()) {
        hasEvents_ = false;
        handleNextEvents();
    }
}

bool DummyEventLoop::hasEvents() {
    std::lock_guard<std::mutex> lock(hasEventsMutex_);
    return hasEvents_;
}

void DummyEventLoop::eventPosted() {
    std::lock_guard<std::mutex> lock(hasEventsMutex_);
    hasEvents_ = true;
}

}
