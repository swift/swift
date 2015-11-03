/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Notifier/NotificationCenterNotifier.h>

#include <map>
#include <string>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>

#import <Cocoa/Cocoa.h>

#include <SwifTools/Notifier/NotificationCenterNotifierDelegate.h>
#include <SwifTools/Cocoa/CocoaUtil.h>

namespace {
	struct Context {
		Context(const boost::function<void()>& callback) : callback(new boost::function<void()>(callback)) {
		}

		~Context() {
			delete callback;
		}

		boost::function<void()>* callback;
	};
}

namespace Swift {

class NotificationCenterNotifier::Private {
	public:
		std::map<std::string, boost::shared_ptr<Context> > callbacksForNotifications;
		boost::intrusive_ptr<NotificationCenterNotifierDelegate> delegate;
};

NotificationCenterNotifier::NotificationCenterNotifier() {
	p = boost::make_shared<Private>();
	p->delegate = boost::intrusive_ptr<NotificationCenterNotifierDelegate>([[NotificationCenterNotifierDelegate alloc] init], false);
	[p->delegate.get() setNotifier: this];

	[[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate: p->delegate.get()];
}

NotificationCenterNotifier::~NotificationCenterNotifier() {
	[[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate: nil];
	p->callbacksForNotifications.clear();
}

void NotificationCenterNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void ()> callback) {
	std::vector<Notifier::Type> defaultTypes = getDefaultTypes();
	if (std::find(defaultTypes.begin(), defaultTypes.end(), type) == defaultTypes.end()) {
		return;
	}
	NSImage* image = [[NSImage alloc] initWithContentsOfFile: STD2NSSTRING(picture.string())];
	NSUserNotification* notification = [[NSUserNotification alloc] init];
	[notification setTitle:STD2NSSTRING(typeToString(type))];
	[notification setSubtitle:STD2NSSTRING(subject)];
	[notification setInformativeText:STD2NSSTRING(description)];
	[notification setContentImage: image];
	[image release];

	// The OS X Notification Center API does not allow to attach custom data, like a pointer to a callback function,
	// to the NSUserNotification object. Therefore we maintain a mapping from a NSUserNotification instance's identification
	// to their respective callbacks.
	[notification setIdentifier:[[NSUUID UUID] UUIDString]];

	/// \todo Currently the elements are only removed on application exit. Ideally the notifications not required anymore
	///       are removed from the map; e.g. when visiting a chat view, all notifications from that view can be removed from
	///       the map and the NSUserNotificationCenter.
	p->callbacksForNotifications[NS2STDSTRING(notification.identifier)] = boost::make_shared<Context>(callback);
	[[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
	[notification release];
}

void NotificationCenterNotifier::purgeCallbacks() {
	p->callbacksForNotifications.clear();
}

void NotificationCenterNotifier::handleUserNotificationActivated(const std::string& identifier) {
	if (p->callbacksForNotifications.find(identifier) != p->callbacksForNotifications.end()) {
		(*p->callbacksForNotifications[identifier]->callback)();
	}
	else {
		SWIFT_LOG(warning) << "Missing callback entry for activated notification. The activate notification may come from another instance." << std::endl;
	}
}

}
