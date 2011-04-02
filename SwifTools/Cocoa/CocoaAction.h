/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
