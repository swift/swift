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

		NSBundle* bundle = [NSBundle bundleForClass: [self class]];
		NSString* path = [bundle pathForResource: @"Offline" ofType:@"png"];
		menuIcon = [[NSImage alloc] initWithContentsOfFile: path];
		[statusItem setImage: menuIcon];

		[self updateMenu];
	}
	return self;
}

- (void) dealloc {
	[statusItem release];
	[menuIcon release];
	[super dealloc];
}

- (void) updateMenu {
	NSMenuItem* statusMenuItem = [[NSMenuItem alloc] initWithTitle: @"Online Users" action: NULL keyEquivalent: @""];
	[statusMenu addItem: statusMenuItem];
	[statusMenu addItem: [NSMenuItem separatorItem]];

	NSMenuItem* loggedInItem = [[NSMenuItem alloc] initWithTitle: @"You are not logged in" action: NULL keyEquivalent: @""];
	[statusMenu addItem: loggedInItem];
	[statusMenu addItem: [NSMenuItem separatorItem]];

	NSMenuItem* exitMenuItem = [[NSMenuItem alloc] initWithTitle: @"Exit" action: @selector(terminate:) keyEquivalent: @""];
	[exitMenuItem setTarget: [NSApplication sharedApplication]];
	[statusMenu addItem: exitMenuItem];
}

@end
