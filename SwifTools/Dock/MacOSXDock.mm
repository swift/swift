/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Dock/MacOSXDock.h>

#include <boost/lexical_cast.hpp>

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

#include <Swiften/Base/String.h>

namespace Swift {

MacOSXDock::MacOSXDock(CocoaApplication*) {
}

void MacOSXDock::setNumberOfPendingMessages(int i) {
    std::string label(i > 0 ? boost::lexical_cast<std::string>(i) : "");
    NSString *labelString = [[NSString alloc] initWithUTF8String: label.c_str()];
    [[NSApp dockTile] setBadgeLabel: labelString];
    [labelString release];
    [NSApp requestUserAttention: NSInformationalRequest];
}

}
