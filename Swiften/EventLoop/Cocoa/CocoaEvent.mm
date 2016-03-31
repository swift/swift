/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/Cocoa/CocoaEvent.h>

#include <Swiften/EventLoop/Cocoa/CocoaEventLoop.h>

@implementation CocoaEvent

- (id) init:(Swift::CocoaEventLoop*) el {
    self = [super init];
    if (self != nil) {
        eventLoop = el;
    }
    return self;
}

- (void) process {
    eventLoop->handleNextCocoaEvent();
}

- (void) dealloc {
    [super dealloc];
}

@end
