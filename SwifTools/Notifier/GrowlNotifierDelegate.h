/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
