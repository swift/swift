#include <Swiften/EventLoop/Cocoa/CocoaEventLoop.h>
#include <Swiften/EventLoop/Cocoa/CocoaEvent.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

CocoaEventLoop::CocoaEventLoop() {
}

void CocoaEventLoop::post(const Event& event) {
	Event* eventCopy = new Event(event);
	CocoaEvent* cocoaEvent = [[CocoaEvent alloc] initWithEvent: eventCopy eventLoop: this];
	[cocoaEvent
			performSelectorOnMainThread:@selector(process) 
			withObject: nil
			waitUntilDone: NO];
	[cocoaEvent release];
}

}
