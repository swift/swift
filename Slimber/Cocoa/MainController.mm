#include "MainController.h"
#include "Slimber.h"

@implementation MainController

- (void) dealloc {
	delete slimber;
	[super dealloc];
}

- (void) awakeFromNib {
	slimber = new Slimber();
}

@end
