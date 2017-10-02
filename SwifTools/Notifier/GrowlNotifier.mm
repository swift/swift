/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Notifier/GrowlNotifier.h>

#include <memory>
#include <set>

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
        std::set<Context*> pendingNotifications;
        boost::intrusive_ptr<GrowlNotifierDelegate> delegate;
};

GrowlNotifier::GrowlNotifier(const std::string& name) : p(new Private()){
    p->delegate = boost::intrusive_ptr<GrowlNotifierDelegate>([[GrowlNotifierDelegate alloc] init], false);
    p->delegate.get().notifier = this;
    p->delegate.get().name = std2NSString(name);

    NSMutableArray* allNotifications = [[NSMutableArray alloc] init];
    foreach(Type type, getAllTypes()) {
        [allNotifications addObject: std2NSString(typeToString(type))];
    }

    NSMutableArray* defaultNotifications = [[NSMutableArray alloc] init];
    foreach(Type type, getDefaultTypes()) {
        [defaultNotifications addObject: std2NSString(typeToString(type))];
    }

    p->delegate.get().registrationDictionary = [[[NSDictionary alloc]
            initWithObjects: [NSArray arrayWithObjects: allNotifications, defaultNotifications, nil]
            forKeys: [NSArray arrayWithObjects: GROWL_NOTIFICATIONS_ALL, GROWL_NOTIFICATIONS_DEFAULT, nil]] autorelease];

    [allNotifications release];
    [defaultNotifications release];

    [GrowlApplicationBridge setGrowlDelegate: p->delegate.get()];
}

GrowlNotifier::~GrowlNotifier() {
    [GrowlApplicationBridge setGrowlDelegate: nil];
    clearPendingNotifications();
}

void GrowlNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picturePath, boost::function<void()> callback) {
    ByteArray picture;
    readByteArrayFromFile(picture, picturePath);

    Context* context = new Context(callback);
    // Growl sometimes sends timeout notifications twice for the same message. We therefore need
    // to keep track of which ones have already been processed.
    p->pendingNotifications.insert(context);

    [GrowlApplicationBridge
        notifyWithTitle: std2NSString(subject)
        description: std2NSString(description)
        notificationName: std2NSString(typeToString(type))
        iconData: [NSData dataWithBytes: vecptr(picture) length: picture.size()]
        priority: 0
        isSticky: NO
        clickContext: [NSData dataWithBytes: &context length: sizeof(context)]];
}

void GrowlNotifier::handleNotificationClicked(void* rawData) {
    Context* context = *(Context**) [((NSData*) rawData) bytes];
    if (p->pendingNotifications.erase(context) > 0) {
        if (!context->callback->empty()) {
            (*context->callback)();
        }
        delete context;
    }
}

void GrowlNotifier::handleNotificationTimedOut(void* rawData) {
    Context* context = *(Context**) [((NSData*) rawData) bytes];
    if (p->pendingNotifications.erase(context) > 0) {
        delete context;
    }
}

bool GrowlNotifier::isExternallyConfigured() const {
    return ![GrowlApplicationBridge isMistEnabled];
}

void GrowlNotifier::purgeCallbacks() {
    clearPendingNotifications();
}

void GrowlNotifier::clearPendingNotifications() {
    foreach (Context* context, p->pendingNotifications) {
        delete context;
    }
    p->pendingNotifications.clear();
}

}
