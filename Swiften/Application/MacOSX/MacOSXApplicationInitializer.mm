#include "Swiften/Application/MacOSX/MacOSXApplicationInitializer.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

namespace Swift {

class MacOSXApplicationInitializer::Private {
	public:
		NSAutoreleasePool* autoReleasePool_;
};

MacOSXApplicationInitializer::MacOSXApplicationInitializer() {
	d = new MacOSXApplicationInitializer::Private();
	NSApplicationLoad();
	d->autoReleasePool_ = [[NSAutoreleasePool alloc] init];
}

MacOSXApplicationInitializer::~MacOSXApplicationInitializer() {
	[d->autoReleasePool_ release];
	delete d;
}

}
