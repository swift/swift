/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/EventViewer/QtEventWindow.h>

#include <QBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeView>

#include <Swiften/Base/Platform.h>

#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

#include <Swift/QtUI/QtSubscriptionRequestWindow.h>

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
    std::shared_ptr<MessageEvent> messageEvent = std::dynamic_pointer_cast<MessageEvent>(event->getEvent());
    std::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = std::dynamic_pointer_cast<SubscriptionRequestEvent>(event->getEvent());
    std::shared_ptr<MUCInviteEvent> mucInviteEvent = std::dynamic_pointer_cast<MUCInviteEvent>(event->getEvent());
    std::shared_ptr<IncomingFileTransferEvent> incomingFTEvent = std::dynamic_pointer_cast<IncomingFileTransferEvent>(event->getEvent());
    std::shared_ptr<ErrorEvent> errorEvent = std::dynamic_pointer_cast<ErrorEvent>(event->getEvent());

    if (messageEvent) {
        if (messageEvent->getStanza()->getType() == Message::Groupchat) {
            eventStream_->send(std::make_shared<JoinMUCUIEvent>(messageEvent->getStanza()->getFrom().toBare(), messageEvent->getStanza()->getTo().getResource()));
        } else {
            eventStream_->send(std::make_shared<RequestChatUIEvent>(messageEvent->getStanza()->getFrom()));
        }
    } else if (subscriptionEvent) {
        QtSubscriptionRequestWindow* window = QtSubscriptionRequestWindow::getWindow(subscriptionEvent, this);
        window->show();
    } else if (mucInviteEvent) {
        eventStream_->send(std::make_shared<RequestChatUIEvent>(mucInviteEvent->getInviter()));
        mucInviteEvent->conclude();
    } else if (incomingFTEvent) {
        eventStream_->send(std::make_shared<RequestChatUIEvent>(incomingFTEvent->getSender()));
        incomingFTEvent->conclude();
    } else {
        if (errorEvent) {
            errorEvent->conclude();
        }
        QMessageBox msgBox;
        msgBox.setText(event->data(Qt::DisplayRole).toString());
        msgBox.exec();
    }

}

void QtEventWindow::addEvent(std::shared_ptr<StanzaEvent> event, bool active) {
    view_->clearSelection();
    model_->addEvent(event, active);
    emit onNewEventCountUpdated(model_->getNewEventCount());
    readButton_->setEnabled(model_->rowCount() > 0);
}

void QtEventWindow::removeEvent(std::shared_ptr<StanzaEvent> event) {
    view_->clearSelection();
    model_->removeEvent(event);
    emit onNewEventCountUpdated(model_->getNewEventCount());
    readButton_->setEnabled(model_->rowCount() > 0);
}

}
