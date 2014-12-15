/*
 * Copyright (c) 2010-2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

// Fix Boost-Cocoa conflict
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Cocoa/Cocoa.h>

class MainController;
class CocoaMenulet;

@interface CocoaController : NSObject

@end
