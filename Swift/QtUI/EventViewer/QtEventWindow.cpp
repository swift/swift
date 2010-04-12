
/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/EventViewer/QtEventWindow.h"

#include <QtDebug>

#include "Swiften/Events/MessageEvent.h"
#include "Swiften/Events/ErrorEvent.h"
#include "Swift/QtUI/QtSubscriptionRequestWindow.h"
#include "Swiften/Events/SubscriptionRequestEvent.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"


#include "Swiften/Base/Platform.h"

namespace Swift {

QtEventWindow::QtEventWindow(UIEventStream* eventStream, QWidget* parent) : QTreeView(parent) {
	eventStream_ = eventStream;
	model_ = new EventModel();
	setModel(model_);
	delegate_ = new EventDelegate();
	setItemDelegate(delegate_);
	setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	setAlternatingRowColors(true);
#endif
	setAnimated(true);
	setIndentation(0);
	setRootIsDecorated(true);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
}

QtEventWindow::~QtEventWindow() {
	delete model_;
	delete delegate_;
}

void QtEventWindow::handleItemActivated(const QModelIndex& item) {
	QtEvent* event = model_->getItem(item.row());
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event->getEvent());
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event->getEvent());
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event->getEvent());
	
	if (messageEvent) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(messageEvent->getStanza()->getFrom())));
	} else if (subscriptionEvent) {
		QtSubscriptionRequestWindow* window = QtSubscriptionRequestWindow::getWindow(subscriptionEvent, this);
		window->show();
	} else if (errorEvent) {
		errorEvent->onConclusion();
	} else {
		qWarning() << "Trying to activate an unexpected event";
	}

}

void QtEventWindow::addEvent(boost::shared_ptr<StanzaEvent> event, bool active) {
	model_->addEvent(event, active);
	emit onNewEventCountUpdated(model_->getNewEventCount());
}

void QtEventWindow::removeEvent(boost::shared_ptr<StanzaEvent> event) {
	model_->removeEvent(event);
	emit onNewEventCountUpdated(model_->getNewEventCount());
}

}
