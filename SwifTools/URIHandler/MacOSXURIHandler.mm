/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/URIHandler/MacOSXURIHandler.h>

#include <Cocoa/Cocoa.h>
#include <iostream>

using namespace Swift;

@interface MacOSXURIEventHandler : NSObject {
	URIHandler* handler;
}
- (id) initWithHandler: (URIHandler*) handler;
- (void) getUrl: (NSAppleEventDescriptor*) event withReplyEvent: (NSAppleEventDescriptor*) replyEvent;

@end
@implementation MacOSXURIEventHandler
	- (id) initWithHandler: (URIHandler*) h {
		if ([super init]) {
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

MacOSXURIHandler::MacOSXURIHandler() {
	p = new Private();
	p->eventHandler = [[MacOSXURIEventHandler alloc] initWithHandler: this]; 
}

MacOSXURIHandler::~MacOSXURIHandler() {
	[p->eventHandler release];
	delete p;
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
