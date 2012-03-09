/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#import <Growl/Growl.h>

namespace Swift {
	class GrowlNotifier;
}

@interface GrowlNotifierDelegate : NSObject<GrowlApplicationBridgeDelegate> {
	Swift::GrowlNotifier* notifier;
	NSString* name;
	NSDictionary* registrationDictionary;
}

@property (nonatomic, retain) NSDictionary* registrationDictionary;
@property (nonatomic, copy) NSString* name;
@property (nonatomic) Swift::GrowlNotifier* notifier;

- (NSDictionary*) registrationDictionaryForGrowl;
- (NSString *) applicationNameForGrowl;
- (void) growlNotificationWasClicked: (id) clickContext;
- (void) growlNotificationTimedOut: (id) clickContext;


@end
