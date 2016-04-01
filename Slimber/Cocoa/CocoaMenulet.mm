/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Slimber/Cocoa/CocoaMenulet.h>

#include <boost/function.hpp>

#include <SwifTools/Cocoa/CocoaUtil.h>

CocoaMenulet::CocoaMenulet() {
    restartAction = [[CocoaAction alloc] initWithFunction:
            new boost::function<void()>(boost::ref(onRestartClicked))];
    menu = [[NSMenu alloc] init];

    statusItem = [[[NSStatusBar systemStatusBar]
            statusItemWithLength: NSVariableStatusItemLength] retain];
    [statusItem setHighlightMode: YES];
    [statusItem setEnabled: YES];
    [statusItem setToolTip: @"Slimber"];
    [statusItem setMenu: menu];
}

CocoaMenulet::~CocoaMenulet() {
    [statusItem release];
    [menu release];
    [restartAction release];
}

void CocoaMenulet::setIcon(const std::string& icon) {
    NSString* path = [[NSBundle mainBundle] pathForResource: std2NSString(icon) ofType:@"png"];
    NSImage* image = [[NSImage alloc] initWithContentsOfFile: path];
    [statusItem setImage: image];
    [image release];
}

void CocoaMenulet::clear() {
    while ([menu numberOfItems] > 0) {
        [menu removeItemAtIndex: 0];
    }
}

void CocoaMenulet::addItem(const std::string& name, const std::string& icon) {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle: std2NSString(name)
        action: nullptr keyEquivalent: @""];
    if (!icon.empty()) {
        NSString* path = [[NSBundle mainBundle] pathForResource: std2NSString(icon) ofType:@"png"];
        NSImage* image = [[NSImage alloc] initWithContentsOfFile: path];
        [item setImage: image];
        [image release];
    }
    [menu addItem: item];
    [item release];
}

void CocoaMenulet::addAboutItem() {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle: @"About Slimber" action: @selector(orderFrontStandardAboutPanel:) keyEquivalent: @""];
    [item setTarget: [NSApplication sharedApplication]];
    [menu addItem: item];
    [item release];
}

void CocoaMenulet::addRestartItem() {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:
            @"Restart" action: @selector(doAction:) keyEquivalent: @""];
    [item setTarget: restartAction];
    [menu addItem: item];
    [item release];
}

void CocoaMenulet::addExitItem() {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle: @"Exit" action: @selector(terminate:) keyEquivalent: @""];
    [item setTarget: [NSApplication sharedApplication]];
    [menu addItem: item];
    [item release];
}

void CocoaMenulet::addSeparator() {
    [menu addItem: [NSMenuItem separatorItem]];
}
