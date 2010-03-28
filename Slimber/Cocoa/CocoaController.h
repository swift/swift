#include <Cocoa/Cocoa.h>

class MainController;
class CocoaMenulet;

@interface CocoaController : NSObject {
	CocoaMenulet* menulet;
	MainController* main;
}

@end
