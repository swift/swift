/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/URIHandler/MacOSXURIHandler.h>

#include <iostream>

#include <Cocoa/Cocoa.h>

using namespace Swift;

@interface MacOSXURIEventHandler : NSObject {
}

- (id) initWithHandler: (URIHandler*) handler;
- (void) getUrl: (NSAppleEventDescriptor*) event withReplyEvent: (NSAppleEventDescriptor*) replyEvent;

@end

@implementation MacOSXURIEventHandler
    {
        URIHandler* handler;
    }

    - (id) initWithHandler: (URIHandler*) h {
        if ((self = [super init])) {
            handler = h;
        }
        return self;
    }

    - (void) getUrl: (NSAppleEventDescriptor*) event withReplyEvent: (NSAppleEventDescriptor*) replyEvent {
        (void) replyEvent;
        NSString* url = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
        handler->onURI(std::string([url UTF8String]));
    }
@end

class MacOSXURIHandler::Private {
    public:
        MacOSXURIEventHandler* eventHandler;
};

MacOSXURIHandler::MacOSXURIHandler() : p(new Private()) {
    p->eventHandler = [[MacOSXURIEventHandler alloc] initWithHandler: this];
}

MacOSXURIHandler::~MacOSXURIHandler() {
    [p->eventHandler release];
}

void MacOSXURIHandler::start() {
    [[NSAppleEventManager sharedAppleEventManager] setEventHandler:p->eventHandler andSelector:@selector(getUrl:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];

    // Register ourselves as default URI handler
    //NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
    //LSSetDefaultHandlerForURLScheme((CFStringRef)@"xmpp", (CFStringRef)bundleID);
}

void MacOSXURIHandler::stop() {
    [[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kInternetEventClass andEventID:kAEGetURL];
}
