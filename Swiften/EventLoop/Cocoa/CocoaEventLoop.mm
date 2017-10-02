/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/Cocoa/CocoaEventLoop.h>
#include <Swiften/EventLoop/Cocoa/CocoaEvent.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

CocoaEventLoop::CocoaEventLoop() : isEventInCocoaEventLoop_(false) {
}

CocoaEventLoop::~CocoaEventLoop() {

}

void CocoaEventLoop::handleNextCocoaEvent() {
    {
        std::unique_lock<std::recursive_mutex> lock(isEventInCocoaEventLoopMutex_);
        isEventInCocoaEventLoop_ = false;
    }
    handleNextEvents();
}

void CocoaEventLoop::eventPosted() {
    std::unique_lock<std::recursive_mutex> lock(isEventInCocoaEventLoopMutex_);
    if (!isEventInCocoaEventLoop_) {
        isEventInCocoaEventLoop_ = true;

        CocoaEvent* cocoaEvent = [[CocoaEvent alloc] init: this];
        [cocoaEvent
            performSelectorOnMainThread:@selector(process)
            withObject: nil
            waitUntilDone: NO];
        [cocoaEvent release];
    }
}

}
