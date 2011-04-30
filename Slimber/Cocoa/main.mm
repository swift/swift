#include <Cocoa/Cocoa.h>

#include "Slimber/Cocoa/main.h"
#include <Swiften/EventLoop/Cocoa/CocoaEventLoop.h>

Swift::CocoaEventLoop* eventLoop;

int main(int argc, char *argv[]) {
	eventLoop = new Swift::CocoaEventLoop();
	int result = NSApplicationMain(argc,  const_cast<const char **>(argv));
	delete eventLoop;
	return result;
}
