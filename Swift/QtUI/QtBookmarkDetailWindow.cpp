/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtBookmarkDetailWindow.h"
#include "QtSwiftUtil.h"
#include <QMessageBox>

namespace Swift {

QtBookmarkDetailWindow::QtBookmarkDetailWindow(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	//connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
	//connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
}

void QtBookmarkDetailWindow::accept() {
	if (commit()) {
		QDialog::accept();
	}
}

boost::optional<MUCBookmark> QtBookmarkDetailWindow::createBookmarkFromForm() {
	//check room
	//check bookmarkName
	JID room(Q2PSTRING(room_->text()));
	if (!room.isValid() || room.getNode().empty() || !room.getResource().empty()) {
		QMessageBox::warning(this, tr("Bookmark not valid"), tr("You must specify a valid room address (e.g. someroom@rooms.example.com)."));
		return boost::optional<MUCBookmark>();
	}
	std::string name(Q2PSTRING(name_->text()));
	if (name.empty()) {
		name = room.toString();
	}

	MUCBookmark bookmark(room, name);
	std::string nick(Q2PSTRING(nick_->text()));
	std::string password(Q2PSTRING(password_->text()));
	bookmark.setAutojoin(autojoin_->isChecked());
	if (!nick.empty()) {
		bookmark.setNick(nick);
	}
	if (!password.empty()) {
		bookmark.setPassword(password);
	}
	return bookmark;
}

}
