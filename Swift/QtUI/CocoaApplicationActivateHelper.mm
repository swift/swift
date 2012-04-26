/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Cocoa/Cocoa.h>
#include "CocoaApplicationActivateHelper.h"

#include <boost/function.hpp>
#include <QApplication>

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

CocoaApplicationActivateHelper::CocoaApplicationActivateHelper() {
	p = new Private();
	p->delegate = [[CocoaApplicationActivateHelperDelegate alloc] init];
	p->initialized = false;
	qApp->installEventFilter(this);
}

CocoaApplicationActivateHelper::~CocoaApplicationActivateHelper() {
	[[NSAppleEventManager sharedAppleEventManager] removeEventHandlerForEventClass:kCoreEventClass andEventID:kAEReopenApplication];
	[p->delegate release];
	delete p;
}

bool CocoaApplicationActivateHelper::eventFilter(QObject* object, QEvent* event) {
	if (object == qApp && event->type() == QEvent::ApplicationActivate && !p->initialized) {
		[[NSAppleEventManager sharedAppleEventManager] setEventHandler:p->delegate andSelector:@selector(handleActivate:withReply:) forEventClass:kCoreEventClass andEventID:kAEReopenApplication];
		p->initialized = true;
	}
	return QObject::eventFilter(object, event);
}


}
