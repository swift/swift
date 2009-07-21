#import "Menulet.h"

@implementation Menulet

- (id) init {
	if ([super init]) {
		statusMenu = [[NSMenu alloc] init];

		statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength: NSVariableStatusItemLength] retain];
		[statusItem setHighlightMode: YES];
		[statusItem setEnabled: YES];
		[statusItem setToolTip: @"Slimber"];	
		[statusItem setMenu: statusMenu];

		userNames = [[NSArray alloc] init];
		selfOnline = NO;

		[self updateMenu];
	}
	return self;
}

- (void) dealloc {
	[statusItem release];
	[menuIcon release];
	[super dealloc];
}

- (void) updateIcon: (BOOL) online {
	NSBundle* bundle = [NSBundle bundleForClass: [self class]];
	NSString* path;
	if (online) {
		path = [bundle pathForResource: @"UsersOnline" ofType:@"png"];
	}
	else {
		path = [bundle pathForResource: @"UsersOffline" ofType:@"png"];
	}
	[statusItem setImage: [[NSImage alloc] initWithContentsOfFile: path]];
}

- (void) updateMenu {
	// Clear the menu
	while ([statusMenu numberOfItems] > 0) {
		[statusMenu removeItemAtIndex: 0];
	}

	// User items
	if ([userNames count] > 0) {
		[statusMenu addItem: [[NSMenuItem alloc] initWithTitle: @"Online users:" action: NULL keyEquivalent: @""]];
		int i;
		for (i = 0; i < [userNames count]; ++i) {
			NSMenuItem* userItem = [[NSMenuItem alloc] initWithTitle: [@"  " stringByAppendingString: [userNames objectAtIndex: i]] action: NULL keyEquivalent: @""];
			[statusMenu addItem: userItem];
		}
	}
	else {
		[statusMenu addItem: [[NSMenuItem alloc] initWithTitle: @"No online users" action: NULL keyEquivalent: @""]];
	}
	[self updateIcon: [userNames count] > 0];
	[statusMenu addItem: [NSMenuItem separatorItem]];

	// Self item
	NSMenuItem* loggedInItem;
	NSBundle* bundle = [NSBundle bundleForClass: [self class]];
	NSString* path;
	if (selfOnline) {
		loggedInItem = [[NSMenuItem alloc] initWithTitle: @"You are logged in" action: NULL keyEquivalent: @""];
		path = [bundle pathForResource: @"Online" ofType:@"png"];
	}
	else {
		loggedInItem = [[NSMenuItem alloc] initWithTitle: @"You are not logged in" action: NULL keyEquivalent: @""];
		path = [bundle pathForResource: @"Offline" ofType:@"png"];
	}
	[loggedInItem setImage: [[NSImage alloc] initWithContentsOfFile: path]];
	[statusMenu addItem: loggedInItem];
	[statusMenu addItem: [NSMenuItem separatorItem]];

	// Exit item
	NSMenuItem* exitMenuItem = [[NSMenuItem alloc] initWithTitle: @"Exit" action: @selector(terminate:) keyEquivalent: @""];
	[exitMenuItem setTarget: [NSApplication sharedApplication]];
	[statusMenu addItem: exitMenuItem];
}

- (void) setSelfConnected: (BOOL) online {
	selfOnline = online;
	[self updateMenu];
}

- (void) setUserNames: (NSArray*) names {
	[names retain];
	[userNames release];
	userNames = names;
	[self updateMenu];
}

@end
