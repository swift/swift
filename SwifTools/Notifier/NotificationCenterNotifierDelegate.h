/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#import <Cocoa/Cocoa.h>

namespace Swift {
    class NotificationCenterNotifier;
}

@interface NotificationCenterNotifierDelegate : NSObject<NSUserNotificationCenterDelegate> {
}

@property (nonatomic) Swift::NotificationCenterNotifier* notifier;

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification;

@end
