/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtBookmarkDetailWindow.h"
#include "QtSwiftUtil.h"

namespace Swift {

QtBookmarkDetailWindow::QtBookmarkDetailWindow(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	//connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
	//connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
}

void QtBookmarkDetailWindow::accept() {
	commit();
	QDialog::accept();
}

boost::shared_ptr<MUCBookmark> QtBookmarkDetailWindow::createBookmarkFromForm() {
	//check room
	//check bookmarkName
	JID room(Q2PSTRING(room_->text()));
	String name(Q2PSTRING(name_->text()));
	String nick(Q2PSTRING(nick_->text()));
	String password(Q2PSTRING(password_->text()));
	bool autojoin = autojoin_->isChecked();
	boost::shared_ptr<MUCBookmark> bookmark(new MUCBookmark(room, name));
	if (!nick.isEmpty()) {
		bookmark->setNick(nick);
	}
	if (!password.isEmpty()) {
		bookmark->setPassword(password);
	}
	bookmark->setAutojoin(autojoin);
	return bookmark;
}

}
