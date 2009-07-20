#pragma once

#include <Cocoa/Cocoa.h>

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
