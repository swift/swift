/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#import "SwifTools/Notifier/NotificationCenterNotifierDelegate.h"

#include <string>

#include <SwifTools/Cocoa/CocoaUtil.h>
#include <SwifTools/Notifier/NotificationCenterNotifier.h>

@implementation NotificationCenterNotifierDelegate

using namespace Swift;

@synthesize notifier;

- (void)userNotificationCenter:(NSUserNotificationCenter *) center didActivateNotification:(NSUserNotification *)notification {
    (void)center;
    std::string identifier = ns2StdString(notification.identifier);
    notifier->handleUserNotificationActivated(identifier);
}

@end
