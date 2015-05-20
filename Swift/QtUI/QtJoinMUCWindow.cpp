/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtJoinMUCWindow.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <QToolTip>

#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {


QtJoinMUCWindow::QtJoinMUCWindow(UIEventStream* uiEventStream) : uiEventStream(uiEventStream) {
	ui.setupUi(this);
#if QT_VERSION >= 0x040700
	ui.room->setPlaceholderText(tr("someroom@rooms.example.com"));
#endif
	connect(ui.room, SIGNAL(returnPressed()), this, SLOT(handleJoin()));
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(handleSearch()));
	connect(ui.joinButton, SIGNAL(clicked()), this, SLOT(handleJoin()));
#if QT_VERSION < 0x050200
	// FIXME: Temporarily set focus on the nickName field first, so that the
	// placeholder for the room is visible. This is just because Qt hides 
	// the placeholder when a widget is focused for some reason.
	// Tracked upstream as QTBUG-33237 and fixed with Qt 5.2.0.
	ui.nickName->setFocus();
#endif
	ui.instantRoom->setChecked(true);
	ui.nickName->setValidator(new NickValidator(this));
	ui.room->setValidator(new RoomJIDValidator(this));
}

void QtJoinMUCWindow::handleJoin() {
	if (ui.room->text().isEmpty() || !ui.room->hasAcceptableInput()) {
		QToolTip::showText(ui.room->mapToGlobal(QPoint()), tr("Please enter a valid room address."), ui.room);
		return;
	}
	if (ui.nickName->text().isEmpty() || !ui.nickName->hasAcceptableInput()) {
		QToolTip::showText(ui.nickName->mapToGlobal(QPoint()), tr("Please enter a valid nickname."), ui.nickName);
		return;
	}

	lastSetNick = Q2PSTRING(ui.nickName->text());
	std::string password = Q2PSTRING(ui.password->text());
	JID room(Q2PSTRING(ui.room->text()));
	uiEventStream->send(boost::make_shared<JoinMUCUIEvent>(room, password, lastSetNick, ui.joinAutomatically->isChecked(), !ui.instantRoom->isChecked()));
	hide();
}

void QtJoinMUCWindow::handleSearch() {
	onSearchMUC();
}

void QtJoinMUCWindow::setNick(const std::string& nick) {
	ui.nickName->setText(P2QSTRING(nick));
	lastSetNick = nick;
}

void QtJoinMUCWindow::setMUC(const std::string& nick) {
	ui.room->setText(P2QSTRING(nick));
}

void QtJoinMUCWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
	ui.password->setText("");
}

}
