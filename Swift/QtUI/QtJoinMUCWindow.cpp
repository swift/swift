/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtJoinMUCWindow.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtJoinMUCWindow::QtJoinMUCWindow() {
	ui.setupUi(this);
#if QT_VERSION >= 0x040700
	ui.room->setPlaceholderText(tr("someroom@rooms.example.com"));
#endif
	connect(ui.room, SIGNAL(returnPressed()), this, SLOT(handleJoin()));
	connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(handleSearch()));
	connect(ui.joinButton, SIGNAL(clicked()), this, SLOT(handleJoin()));
	// FIXME: Temporarily set focus on the nickName field first, so that the
	// placeholder for the room is visible. This is just because Qt hides 
	// the placeholder when a widget is focused for some reason.
	ui.nickName->setFocus();
}

void QtJoinMUCWindow::handleJoin() {
	if (ui.room->text().isEmpty()) {
		// TODO: Error
		return;
	}
	if (ui.nickName->text().isEmpty()) {
		// TODO: Error
		return;
	}

	lastSetNick = Q2PSTRING(ui.nickName->text());
	JID room(Q2PSTRING(ui.room->text()));
	onJoinMUC(room, lastSetNick, ui.joinAutomatically->isChecked()); 
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
}

}
