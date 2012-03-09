/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#import "GrowlNotifierDelegate.h"

#include <SwifTools/Notifier/GrowlNotifier.h>

@implementation GrowlNotifierDelegate;

@synthesize registrationDictionary;
@synthesize name;
@synthesize notifier;

using namespace Swift;

- (NSString *) applicationNameForGrowl {
	return name;
}

- (NSDictionary*) registrationDictionaryForGrowl {
	return registrationDictionary;
}

- (void) growlNotificationWasClicked: (id) clickContext {
	notifier->handleNotificationClicked(clickContext);
}

- (void) growlNotificationTimedOut: (id) clickContext {
	notifier->handleNotificationTimedOut(clickContext);
}

@end
