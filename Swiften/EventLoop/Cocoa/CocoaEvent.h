/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Foundation/Foundation.h>

namespace Swift {
	class Event;
	class CocoaEventLoop;
}

// Using deprecated declaration of instance vars in interface, because this
// is required for older runtimes (e.g. 32-bit Mac OS X)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-interface-ivars"

@interface CocoaEvent : NSObject {
	Swift::Event* event;
	Swift::CocoaEventLoop* eventLoop;
}

#pragma clang diagnostic pop

// Takes ownership of event
- (id) initWithEvent: (Swift::Event*) e eventLoop: (Swift::CocoaEventLoop*) el;
- (void) process;
- (void) dealloc;

@end
