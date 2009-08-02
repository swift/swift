#include "Slimber/Cocoa/CocoaAction.h"

@implementation CocoaAction

- (id) initWithFunction: (boost::function<void()>*) f {
    if ([super init]) {
			function = f;
		}
    return self;
}

- (void) dealloc {
	delete function;
	[super dealloc];
}

- (void) doAction: (id) sender {
	(void) sender;
	(*function)();
}

@end
