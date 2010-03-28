// FIXME: Is it safe to pass boost::function<void()> by raw values?
// FIXME: Should we release the strings created in the constructor?

#include <cassert>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Notifier/GrowlNotifier.h"

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace {
	struct Context {
		Context() {}
		Context(const boost::function<void()>& callback) : callback(callback) {}

		boost::function<void()> callback;
	};

	void notificationClicked(CFPropertyListRef growlContext) {
		Context context;

		CFDataRef growlContextData = (CFDataRef) CFArrayGetValueAtIndex((CFArrayRef) growlContext, 0);
		assert(CFDataGetLength(growlContextData) == sizeof(Context));
		CFDataGetBytes(growlContextData, CFRangeMake(0, CFDataGetLength(growlContextData)), (UInt8*) &context);
		
		context.callback();
	}

	void notificationTimedout(CFPropertyListRef) {
	}
}

namespace Swift {

GrowlNotifier::GrowlNotifier(const String& name) {
	// All notifications
	CFMutableArrayRef allNotifications = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(allNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(ContactAvailable)));
	CFArrayAppendValue(allNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(ContactUnavailable)));
	CFArrayAppendValue(allNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(ContactStatusChange)));
	CFArrayAppendValue(allNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(IncomingMessage)));

	// Default Notifications
	CFMutableArrayRef defaultNotifications = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(defaultNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(ContactAvailable)));
	CFArrayAppendValue(defaultNotifications, SWIFTEN_STRING_TO_CFSTRING(typeToString(IncomingMessage)));

	// Initialize delegate
	InitGrowlDelegate(&delegate_);
	delegate_.applicationName = SWIFTEN_STRING_TO_CFSTRING(name);
	CFTypeRef keys[] = { GROWL_NOTIFICATIONS_ALL, GROWL_NOTIFICATIONS_DEFAULT };
	CFTypeRef values[] = { allNotifications, defaultNotifications };
	delegate_.registrationDictionary = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	delegate_.growlNotificationWasClicked = &notificationClicked;
	delegate_.growlNotificationTimedOut = &notificationTimedout;
	Growl_SetDelegate(&delegate_);
}

void GrowlNotifier::showMessage(Type type, const String& subject, const String& description, const ByteArray& picture, boost::function<void()> callback) {
	CFStringRef cfSubject = SWIFTEN_STRING_TO_CFSTRING(subject);
	CFStringRef cfDescription = SWIFTEN_STRING_TO_CFSTRING(description);
	CFStringRef cfName = SWIFTEN_STRING_TO_CFSTRING(typeToString(type));
	CFDataRef cfIcon = CFDataCreate( NULL, (UInt8*) picture.getData(), picture.getSize());
	
	Context context(callback);
	CFDataRef cfContextData[1];
	cfContextData[0] = CFDataCreate(kCFAllocatorDefault, (const UInt8*) &context, sizeof(Context));
	CFArrayRef cfContext = CFArrayCreate( kCFAllocatorDefault, (const void **) cfContextData, 1, &kCFTypeArrayCallBacks );
	CFRelease(cfContextData[0]);

	Growl_NotifyWithTitleDescriptionNameIconPriorityStickyClickContext(cfSubject, cfDescription, cfName, cfIcon, 0, false, cfContext);

	CFRelease(cfContext);
	CFRelease(cfIcon);
	CFRelease(cfName);
	CFRelease(cfDescription);
	CFRelease(cfSubject);
}

String GrowlNotifier::typeToString(Type type) {
	switch (type) {
		case ContactAvailable: return "Contact Becomes Available";
		case ContactUnavailable: return "Contact Becomes Unavailable";
		case ContactStatusChange: return "Contact Changes Status";
		case IncomingMessage: return "Incoming Message";
	}
	assert(false);
	return "";
}

}
