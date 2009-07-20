#import <Cocoa/Cocoa.h>


@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	IBOutlet NSMenu* statusMenu;
	NSImage* menuIcon;
}

-(IBAction) updateStatus: (id) sender;

@end
