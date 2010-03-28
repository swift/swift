#include <Cocoa/Cocoa.h>

#include "Swiften/EventLoop/Cocoa/CocoaEventLoop.h"

int main(int argc, char *argv[]) {
	Swift::CocoaEventLoop eventLoop;
	return NSApplicationMain(argc,  const_cast<const char **>(argv));
}
