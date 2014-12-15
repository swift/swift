/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Cocoa/Cocoa.h>
#include <boost/function.hpp>

@interface CocoaAction : NSObject

/**
 * Acquires ownership of 'f'.
 */
- (id) initWithFunction: (boost::function<void()>*) f;

/**
 * Calls the functor passed as a parameter to the contsructor.
 */
- (void) doAction: (id) sender;

@end
