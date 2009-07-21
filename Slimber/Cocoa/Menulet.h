#include <Cocoa/Cocoa.h>

@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	NSMenu* statusMenu;
	NSImage* menuIcon;
}

- (id) init;
- (void) updateMenu;

@end
