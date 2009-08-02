#pragma once

#include <Cocoa/Cocoa.h>

class CocoaMenulet;

@interface CocoaMenuletDelegate : NSObject {
	CocoaMenulet* menulet;
}

- (id) initWithMenulet: (CocoaMenulet*) m;
- (void) handleRestartClicked: (id) sender;

@end
