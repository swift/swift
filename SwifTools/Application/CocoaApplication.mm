#include <SwifTools/Application/CocoaApplication.h>

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

namespace Swift {

class CocoaApplication::Private {
	public:
		NSAutoreleasePool* autoReleasePool_;
};

CocoaApplication::CocoaApplication() {
	d = new CocoaApplication::Private();
	NSApplicationLoad();
	d->autoReleasePool_ = [[NSAutoreleasePool alloc] init];
}

CocoaApplication::~CocoaApplication() {
	[d->autoReleasePool_ release];
	delete d;
}

}
