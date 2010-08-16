/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Foundation/Foundation.h>

namespace Swift {
	class Event;
	class CocoaEventLoop;
}

@interface CocoaEvent : NSObject {
	Swift::Event* event;
	Swift::CocoaEventLoop* eventLoop;
}

// Takes ownership of event
- (id) initWithEvent: (Swift::Event*) e eventLoop: (Swift::CocoaEventLoop*) el;
- (void) process;
- (void) dealloc;

@end
