/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Cocoa/CocoaAction.h>

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
