/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/DummyEventLoop.h>

#include <Swiften/Base/Log.h>

namespace Swift {

DummyEventLoop::DummyEventLoop() {
}

DummyEventLoop::~DummyEventLoop() {
    if (hasEvents()) {
        SWIFT_LOG(warning) << "DummyEventLoop: Unhandled events at destruction time" << std::endl;
    }
}

void DummyEventLoop::processEvents() {
    while(hasEvents()) {
        hasEvents_ = false;
        handleNextEvents();
    }
}

bool DummyEventLoop::hasEvents() {
    return hasEvents_;
}

void DummyEventLoop::eventPosted() {
    hasEvents_ = true;
}

}
