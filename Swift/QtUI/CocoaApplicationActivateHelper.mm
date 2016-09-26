/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/CocoaApplicationActivateHelper.h>

#include <memory>

#include <boost/function.hpp>

#include <QApplication>

#include <Cocoa/Cocoa.h>

@interface CocoaApplicationActivateHelperDelegate : NSObject {
}
- (void) handleActivate: (NSAppleEventDescriptor*) event withReply: (NSAppleEventDescriptor*) reply;
@end

@implementation CocoaApplicationActivateHelperDelegate
- (void) handleActivate: (NSAppleEventDescriptor*) event withReply: (NSAppleEventDescriptor*) reply {
    (void) event; (void) reply;
    QApplication::postEvent(qApp, new QEvent(QEvent::ApplicationActivate));
}
@end

namespace Swift {

struct CocoaApplicationActivateHelper::Private {
    CocoaApplicationActivateHelperDelegate* delegate;
    bool initialized;
};

CocoaApplicationActivateHelper::CocoaApplicationActivateHelper() : p(new Private()) {
    p->delegate = [[CocoaApplicationActivateHelperDelegate alloc] init];
    p->initialized = false;
    qApp->installEventFilter(this);
}

CocoaApplicationActivateHelper::~CocoaApplicationActivateHelper() {
    [[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kCoreEventClass andEventID:kAEReopenApplication];
    [p->delegate release];
}

bool CocoaApplicationActivateHelper::eventFilter(QObject* object, QEvent* event) {
    if (object == qApp && event->type() == QEvent::ApplicationActivate && !p->initialized) {
        [[NSAppleEventManager sharedAppleEventManager] setEventHandler:p->delegate andSelector:@selector(handleActivate:withReply:) forEventClass:kCoreEventClass andEventID:kAEReopenApplication];
        p->initialized = true;
    }
    return QObject::eventFilter(object, event);
}


}
