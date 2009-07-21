#include <Cocoa/Cocoa.h>

@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	NSMenu* statusMenu;
	NSImage* menuIcon;
	BOOL selfOnline;
}

- (id) init;
- (void) updateMenu;
- (void) setUsersOnline: (BOOL) online;
- (void) setSelfConnected: (BOOL) online;

@end
