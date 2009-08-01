#include <Cocoa/Cocoa.h>

@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	NSMenu* statusMenu;
	NSImage* menuIcon;
	NSArray* userNames;
	BOOL xmppOnline;
	NSString* xmppStatus;
}

- (id) init;
- (void) updateMenu;
- (void) setUserNames: (NSArray*) names;
- (void) setXMPPStatus: (NSString*) status online: (BOOL) online;

@end
