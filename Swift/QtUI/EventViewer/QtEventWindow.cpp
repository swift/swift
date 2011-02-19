
/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/EventViewer/QtEventWindow.h"

#include <QtDebug>
#include <QBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "Swift/Controllers/XMPPEvents/MessageEvent.h"
#include "Swift/Controllers/XMPPEvents/ErrorEvent.h"
#include "Swift/QtUI/QtSubscriptionRequestWindow.h"
#include "Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"


#include "Swiften/Base/Platform.h"

namespace Swift {

QtEventWindow::QtEventWindow(UIEventStream* eventStream) : EventWindow(false) {
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);

	view_ = new QTreeView(this);
	layout->addWidget(view_);
	eventStream_ = eventStream;
	model_ = new EventModel();
	view_->setModel(model_);
	delegate_ = new EventDelegate();
	view_->setItemDelegate(delegate_);
	view_->setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	view_->setAlternatingRowColors(true);
#endif
	view_->setAnimated(true);
	view_->setIndentation(0);
	view_->setRootIsDecorated(true);
	
	readButton_ = new QPushButton(tr("Display Notice"), this);
	layout->addWidget(readButton_);
	readButton_->setEnabled(false);
	connect(readButton_, SIGNAL(clicked()), this, SLOT(handleReadClicked()));
	connect(view_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
	connect(view_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
	
}

QtEventWindow::~QtEventWindow() {
	delete model_;
	delete delegate_;
	/* Not view_ because this is the parent */
}

void QtEventWindow::handleItemClicked(const QModelIndex&) {
	readButton_->setEnabled(true);
}

void QtEventWindow::handleReadClicked() {
	QModelIndex index = view_->currentIndex();
	if (!index.isValid()) {
		return;
	}
	handleItemActivated(index);
}

void QtEventWindow::handleItemActivated(const QModelIndex& item) {
	QtEvent* event = model_->getItem(item.row());
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(event->getEvent());
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(event->getEvent());
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(event->getEvent());
	
	if (messageEvent) {
		if (messageEvent->getStanza()->getType() == Message::Groupchat) {
			eventStream_->send(boost::shared_ptr<UIEvent>(new JoinMUCUIEvent(messageEvent->getStanza()->getFrom().toBare(), messageEvent->getStanza()->getTo().getResource())));
		} else {
			eventStream_->send(boost::shared_ptr<UIEvent>(new RequestChatUIEvent(messageEvent->getStanza()->getFrom())));
		}
	} else if (subscriptionEvent) {
		QtSubscriptionRequestWindow* window = QtSubscriptionRequestWindow::getWindow(subscriptionEvent, this);
		window->show();
	} else {
		if (errorEvent) {
			errorEvent->conclude();
		}
		QMessageBox msgBox;
		msgBox.setText(model_->data(item, Qt::DisplayRole).toString());
		msgBox.exec();
	}

}

void QtEventWindow::addEvent(boost::shared_ptr<StanzaEvent> event, bool active) {
	view_->clearSelection();
	model_->addEvent(event, active);
	emit onNewEventCountUpdated(model_->getNewEventCount());
}

void QtEventWindow::removeEvent(boost::shared_ptr<StanzaEvent> event) {
	view_->clearSelection();
	model_->removeEvent(event);
	emit onNewEventCountUpdated(model_->getNewEventCount());
}

}
