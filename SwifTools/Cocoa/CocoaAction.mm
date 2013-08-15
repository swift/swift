/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Cocoa/CocoaAction.h>

@implementation CocoaAction {
	boost::function<void ()>* function;
}

- (id) initWithFunction: (boost::function<void()>*) f {
	if ((self = [super init])) {
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
