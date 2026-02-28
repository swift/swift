/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Foundation/Foundation.h>

// The following line is a workaround for a bug in Boost 1.60 when building as C++11.
// See ticket #11897 and #11863 in Boost's bug tracker.
#undef check

namespace Swift {
    class CocoaEventLoop;
}

// Using deprecated declaration of instance vars in interface, because this
// is required for older runtimes (e.g. 32-bit Mac OS X)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-interface-ivars"

@interface CocoaEvent : NSObject {
    Swift::CocoaEventLoop* eventLoop;
}

#pragma clang diagnostic pop

// Takes ownership of event
- (id) init:(Swift::CocoaEventLoop*) el;
- (void) process;
- (void) dealloc;

@end
