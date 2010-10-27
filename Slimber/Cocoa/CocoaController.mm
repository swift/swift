#include "Slimber/Cocoa/CocoaController.h"

#include "Slimber/MainController.h"
#include "Slimber/Cocoa/CocoaMenulet.h"
#include "Slimber/Cocoa/main.h"

@implementation CocoaController

- (void) dealloc {
	delete main;
	delete menulet;
	[super dealloc];
}

- (void) awakeFromNib {
	menulet = new CocoaMenulet();
	main = new MainController(menulet, eventLoop);
}

@end
