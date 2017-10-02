/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
