/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/MUCSearch/QtMUCSearchWindow.h"

#include <qdebug.h>
#include <QMovie>
#include <QScrollBar>
#include <QTimer>

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"
#include "Swift/QtUI/MUCSearch/MUCSearchModel.h"
#include "Swift/QtUI/MUCSearch/MUCSearchDelegate.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtMUCSearchWindow::QtMUCSearchWindow(UIEventStream* eventStream) {
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
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
	connect(service_, SIGNAL(activated(const QString&)), this, SLOT(handleSearch(const QString&)));
	connect(room_, SIGNAL(returnPressed()), this, SLOT(handleJoin()));
	connect(nickName_, SIGNAL(returnPressed()), room_, SLOT(setFocus()));
	connect(searchButton_, SIGNAL(clicked()), this, SLOT(handleSearch()));
	connect(joinButton_, SIGNAL(clicked()), this, SLOT(handleJoin()));
	connect(results_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleSelected(const QModelIndex&)));
	connect(results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleActivated(const QModelIndex&)));
	throbber_ = new QLabel("Searching", results_);
	throbber_->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), throbber_));
	throbber_->setToolTip("Searching");
	hasHadScrollBars_ = false;
	updateThrobberPosition();
	setSearchInProgress(false);
}

QtMUCSearchWindow::~QtMUCSearchWindow() {

}

void QtMUCSearchWindow::resizeEvent(QResizeEvent* /*event*/) {
	updateThrobberPosition();
}

void QtMUCSearchWindow::updateThrobberPosition() {
	bool isShown = throbber_->isVisible();
	int resultWidth = results_->width();
	int resultHeight = results_->height();
	//throbberWidth = throbber_->movie()->scaledSize().width();
	//throbberHeight = throbber_->movie()->scaledSize().height();
	int throbberWidth = 16; /* This is nasty, but the above doesn't work! */
	int throbberHeight = 16;
	/* It's difficult (or I spent a while trying) to work out whether the scrollbars are currently shown and their appropriate size,
	 * because if you listen for the expanded/collapsed signals, you seem to get them before the scrollbars are updated.
	 * This seems an acceptable workaround.
	 */
	hasHadScrollBars_ |= results_->verticalScrollBar()->isVisible();
	int hMargin = hasHadScrollBars_ ? results_->verticalScrollBar()->width() + 2 : 2;
	int vMargin = 2; /* We don't get horizontal scrollbars */
	throbber_->setGeometry(QRect(resultWidth - throbberWidth - hMargin, resultHeight - throbberHeight - vMargin, throbberWidth, throbberHeight)); /* include margins */
	throbber_->setVisible(isShown);
}

void QtMUCSearchWindow::addSavedServices(const std::vector<JID>& services) {
	service_->clear();
	foreach (JID jid, services) {
		service_->addItem(P2QSTRING(jid.toString()));
	}
	service_->clearEditText();
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

void QtMUCSearchWindow::handleSearch(const QString& text) {
	if (text.isEmpty()) {
		return;
	}
	onAddService(JID(Q2PSTRING(text)));
}

void QtMUCSearchWindow::handleSearch() {
	handleSearch(service_->currentText());
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
		lastSetNick_ = Q2PSTRING(nickName_->text());
		maybeNick = lastSetNick_;
	}

	JID room(Q2PSTRING(room_->text()));
	if (joinAutomatically_->isChecked()) {
		createAutoJoin(room, maybeNick);
	}
	eventStream_->send(boost::shared_ptr<UIEvent>(new JoinMUCUIEvent(room, maybeNick)));
	hide();
}

void QtMUCSearchWindow::createAutoJoin(const JID& room, boost::optional<String> passedNick) {
	String nick = lastSetNick_;
	if (passedNick) {
		nick = passedNick.get();
	}
	MUCBookmark bookmark(room, room.getNode());
	bookmark.setAutojoin(true);
	if (!nick.isEmpty()) {
		bookmark.setNick(nick);
	}
	//if (!password.isEmpty()) {
	//	bookmark.setPassword(password);
	//}
	eventStream_->send(boost::shared_ptr<UIEvent>(new AddMUCBookmarkUIEvent(bookmark)));
} 

void QtMUCSearchWindow::setNick(const String& nick) {
	nickName_->setText(P2QSTRING(nick));
	lastSetNick_ = nick;
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
	updateThrobberPosition();
	MUCSearchServiceItem* serviceItem = new MUCSearchServiceItem(P2QSTRING(service.getJID().toString()));
	foreach (MUCService::MUCRoom room, service.getRooms()) {
		new MUCSearchRoomItem(P2QSTRING(room.getNode()), serviceItem);
	}
	model_->addService(serviceItem);
	results_->expandAll();
}

void QtMUCSearchWindow::setSearchInProgress(bool searching) {
	if (searching) {
		throbber_->movie()->start();
	} else {
		throbber_->movie()->stop();
	}
	throbber_->setVisible(searching);
}

}
