/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// Fix Boost-Cocoa conflict
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Cocoa/Cocoa.h>

class MainController;
class CocoaMenulet;

@interface CocoaController : NSObject

@end
