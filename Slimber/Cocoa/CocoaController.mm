/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Slimber/Cocoa/CocoaController.h>

#include <Slimber/Cocoa/CocoaMenulet.h>
#include <Slimber/Cocoa/main.h>
#include <Slimber/MainController.h>

@implementation CocoaController {
    CocoaMenulet* menulet;
    MainController* main;
}

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
