#pragma once

#include <Cocoa/Cocoa.h>
#include <boost/function.hpp>

@interface CocoaAction : NSObject {
	boost::function<void ()>* function;
}

/**
 * Acquires ownership of 'f'.
 */
- (id) initWithFunction: (boost::function<void()>*) f;

/**
 * Calls the functor passed as a parameter to the contsructor.
 */
- (void) doAction: (id) sender;

@end
