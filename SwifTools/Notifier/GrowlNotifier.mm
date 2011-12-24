/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Notifier/GrowlNotifier.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <SwifTools/Notifier/GrowlNotifierDelegate.h>
#include <SwifTools/Cocoa/CocoaUtil.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/ByteArray.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace {
	struct Context {
		Context(const boost::function<void()>& callback) : callback(new boost::function<void()>(callback)) {}

		boost::function<void()>* callback;
	};
}

namespace Swift {

class GrowlNotifier::Private {
	public:
		boost::intrusive_ptr<GrowlNotifierDelegate> delegate;
};

GrowlNotifier::GrowlNotifier(const std::string& name) {
	p = boost::make_shared<Private>();
	p->delegate = boost::intrusive_ptr<GrowlNotifierDelegate>([[GrowlNotifierDelegate alloc] init], false);
	p->delegate.get().name = STD2NSSTRING(name);
	
	NSMutableArray* allNotifications = [[NSMutableArray alloc] init];
	foreach(Type type, getAllTypes()) {
		[allNotifications addObject: STD2NSSTRING(typeToString(type))];
	}

	NSMutableArray* defaultNotifications = [[NSMutableArray alloc] init];
	foreach(Type type, getDefaultTypes()) {
		[defaultNotifications addObject: STD2NSSTRING(typeToString(type))];
	}

	p->delegate.get().registrationDictionary = [[[NSDictionary alloc] 
			initWithObjects: [NSArray arrayWithObjects: allNotifications, defaultNotifications, nil] 
			forKeys: [NSArray arrayWithObjects: GROWL_NOTIFICATIONS_ALL, GROWL_NOTIFICATIONS_DEFAULT, nil]] autorelease];
	
	[allNotifications release];
	[defaultNotifications release];

	[GrowlApplicationBridge setGrowlDelegate: p->delegate.get()];
}

void GrowlNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picturePath, boost::function<void()> callback) {
	ByteArray picture;
	readByteArrayFromFile(picture, picturePath.string());

	Context* context = new Context(callback);

	[GrowlApplicationBridge 
		notifyWithTitle: STD2NSSTRING(subject)
		description: STD2NSSTRING(description)
		notificationName: STD2NSSTRING(typeToString(type))
		iconData: [NSData dataWithBytes: vecptr(picture) length: picture.size()]
		priority: 0
		isSticky: NO
		clickContext: [NSData dataWithBytes: &context length: sizeof(context)]];
}

void GrowlNotifier::handleNotificationClicked(void* rawData) {
	Context* context = *(Context**) [((NSData*) rawData) bytes];
	if (!context->callback->empty()) {
		(*context->callback)();
	}
	delete context;
}

void GrowlNotifier::handleNotificationTimedOut(void* rawData) {
	delete *(Context**) [((NSData*) rawData) bytes];
}

bool GrowlNotifier::isExternallyConfigured() const {
	return ![GrowlApplicationBridge isMistEnabled];
}

}
