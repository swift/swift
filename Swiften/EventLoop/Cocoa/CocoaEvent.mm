#include <Swiften/EventLoop/Cocoa/CocoaEvent.h>
#include <Swiften/EventLoop/Event.h>
#include <Swiften/EventLoop/Cocoa/CocoaEventLoop.h>

@implementation CocoaEvent 

- (id) initWithEvent: (Swift::Event*) e eventLoop: (Swift::CocoaEventLoop*) el {
	self = [super init]; 
	if (self != nil) { 
		event = e;
		eventLoop = el;
	}
	return self; 
}

- (void) process {
	eventLoop->handleEvent(*event);
}

- (void) dealloc {
	delete event;
	[super dealloc];
}

@end
