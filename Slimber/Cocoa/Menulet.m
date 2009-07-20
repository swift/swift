#import "Menulet.h"

@implementation Menulet

- (void) dealloc {
	[statusItem release];
	[menuIcon release];
	[super dealloc];
}

- (void) awakeFromNib {
	statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength: NSVariableStatusItemLength] retain];
	[statusItem setHighlightMode: YES];
	[statusItem setEnabled: YES];
	[statusItem setToolTip: @"Slimber"];	
	[statusItem setMenu: statusMenu];
	
	NSBundle* bundle = [NSBundle bundleForClass: [self class]];
	NSString* path = [bundle pathForResource: @"Offline" ofType:@"png"];
	menuIcon = [[NSImage alloc] initWithContentsOfFile: path];
	[statusItem setImage: menuIcon];

	NSMenuItem* statusMenuItem = [[NSMenuItem alloc] initWithTitle: @"Online Users" action: NULL keyEquivalent:@""];
	[statusMenu addItem: statusMenuItem];
	[statusMenu addItem: [NSMenuItem separatorItem]];
	NSMenuItem* loggedInItem = [[NSMenuItem alloc] initWithTitle: @"You are not logged in" action: NULL keyEquivalent:@""];
	[statusMenu addItem: loggedInItem];
}

- (IBAction) updateStatus: (id) sender {
	[statusItem setTitle: [NSString stringWithString:@"Boo"]]; 
	NSMenuItem* statusMenuItem = [[NSMenuItem alloc] initWithTitle: @"Foo" action: @selector(updateStatus:) keyEquivalent:@""];
	[statusMenuItem setTitle:[NSString stringWithString:@"Boo"]];
	[statusMenu insertItem: statusMenuItem atIndex:1];
}

@end
