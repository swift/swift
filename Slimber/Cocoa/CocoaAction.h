#pragma once

#include <Cocoa/Cocoa.h>
#include <boost/function.hpp>

class CocoaMenulet;

@interface CocoaAction : NSObject {
	boost::function<void ()>* function;
}

- (id) initWithFunction: (boost::function<void()>*) f;
- (void) doAction: (id) sender;

@end
