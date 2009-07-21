#include <Cocoa/Cocoa.h>

@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	NSMenu* statusMenu;
	NSImage* menuIcon;
	NSArray* userNames;
	BOOL selfOnline;
}

- (id) init;
- (void) updateMenu;
- (void) setUserNames: (NSArray*) names;
- (void) setSelfConnected: (BOOL) online;

@end
