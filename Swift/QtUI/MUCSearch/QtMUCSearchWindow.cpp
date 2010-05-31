/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/MUCSearch/QtMUCSearchWindow.h"

#include <qdebug.h>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/QtUI/MUCSearch/MUCSearchModel.h"
#include "Swift/QtUI/MUCSearch/MUCSearchDelegate.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtMUCSearchWindow::QtMUCSearchWindow(UIEventStream* eventStream) {
	eventStream_ = eventStream;
	setupUi(this);
	showEmptyRooms_->hide();
	filterLabel_->hide();
	filter_->hide();
	model_ = new MUCSearchModel();
	delegate_ = new MUCSearchDelegate();
	results_->setModel(model_);
	results_->setItemDelegate(delegate_);
	results_->setHeaderHidden(true);
#ifdef SWIFT_PLATFORM_MACOSX
	results_->setAlternatingRowColors(true);
#endif
	connect(service_, SIGNAL(returnPressed()), this, SLOT(handleSearch()));
	connect(searchButton_, SIGNAL(clicked()), this, SLOT(handleSearch()));
	connect(joinButton_, SIGNAL(clicked()), this, SLOT(handleJoin()));
	connect(results_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleSelected(const QModelIndex&)));
	connect(results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleActivated(const QModelIndex&)));
}

QtMUCSearchWindow::~QtMUCSearchWindow() {

}

void QtMUCSearchWindow::handleActivated(const QModelIndex& index) {
	if (!index.isValid()) {
		return;
	}
	MUCSearchRoomItem* roomItem = dynamic_cast<MUCSearchRoomItem*>(static_cast<MUCSearchItem*>(index.internalPointer()));
	if (roomItem) {
		handleSelected(index);
		handleJoin();
	}
}

void QtMUCSearchWindow::handleSelected(const QModelIndex& current) {
	if (!current.isValid()) {
		return;
	}	
	MUCSearchRoomItem* roomItem = dynamic_cast<MUCSearchRoomItem*>(static_cast<MUCSearchItem*>(current.internalPointer()));
	if (roomItem) {
		room_->setText(roomItem->getNode() + "@" + roomItem->getParent()->getHost());
	}

}

void QtMUCSearchWindow::handleSearch() {
	if (service_->text().isEmpty()) {
		return;
	}
	onAddService(JID(Q2PSTRING(service_->text())));
}

void QtMUCSearchWindow::handleJoin() {
	if (room_->text().isEmpty()) {
		handleSelected(results_->currentIndex());
	}
	if (room_->text().isEmpty()) {
		return;
	}
	boost::optional<String> maybeNick;
	if (!nickName_->text().isEmpty()) {
		maybeNick = Q2PSTRING(nickName_->text());
	}
	eventStream_->send(boost::shared_ptr<UIEvent>(new JoinMUCUIEvent(JID(Q2PSTRING(room_->text())), maybeNick)));
	hide();
}

void QtMUCSearchWindow::setNick(const String& nick) {
	nickName_->setText(P2QSTRING(nick));
}

void QtMUCSearchWindow::setMUC(const String& nick) {
	room_->setText(P2QSTRING(nick));
}

void QtMUCSearchWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
}

void QtMUCSearchWindow::clearList() {
	model_->clear();
}

void QtMUCSearchWindow::addService(const MUCService& service) {
	MUCSearchServiceItem* serviceItem = new MUCSearchServiceItem(P2QSTRING(service.getJID().toString()));
	foreach (MUCService::MUCRoom room, service.getRooms()) {
		new MUCSearchRoomItem(P2QSTRING(room.getNode()), serviceItem);
	}
	model_->addService(serviceItem);
}

}
