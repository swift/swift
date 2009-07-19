#import <Cocoa/Cocoa.h>


@interface Menulet : NSObject {
	NSStatusItem* statusItem;
	IBOutlet NSMenu *statusMenu;
}

-(IBAction) updateStatus: (id) sender;

@end