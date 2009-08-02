#include "Slimber/Cocoa/CocoaMenuletDelegate.h"
#include "Slimber/Cocoa/CocoaMenulet.h"

@implementation CocoaMenuletDelegate

- (id) initWithMenulet: (CocoaMenulet*) m {
    if ([super init]) {
			menulet = m;
		}
    return self;
}

- (void) handleRestartClicked: (id) sender {
	menulet->onRestartClicked();
}

@end
