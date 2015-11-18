/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Foundation/Foundation.h>

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
