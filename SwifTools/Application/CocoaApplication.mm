/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Application/CocoaApplication.h>

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

namespace Swift {

class CocoaApplication::Private {
    public:
        NSAutoreleasePool* autoReleasePool_;
};

CocoaApplication::CocoaApplication() : d(new Private()) {
    NSApplicationLoad();
    d->autoReleasePool_ = [[NSAutoreleasePool alloc] init];
}

CocoaApplication::~CocoaApplication() {
    [d->autoReleasePool_ release];
}

}
