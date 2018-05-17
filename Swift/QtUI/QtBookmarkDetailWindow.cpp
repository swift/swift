/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtBookmarkDetailWindow.h>

#include <QMessageBox>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtBookmarkDetailWindow::QtBookmarkDetailWindow(QWidget* parent) : QDialog(parent) {
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    //connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
    //connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
    setFixedHeight(sizeHint().height());
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
    bookmark.setAutojoin(true);
    if (!nick.empty()) {
        bookmark.setNick(nick);
    }
    if (!password.empty()) {
        bookmark.setPassword(password);
    }
    return bookmark;
}

void QtBookmarkDetailWindow::createFormFromBookmark(const MUCBookmark& bookmark) {
    if (bookmark.getRoom().isValid()) {
        room_->setText(P2QSTRING(bookmark.getRoom().toString()));
    }

    if (!bookmark.getName().empty()) {
        name_->setText(P2QSTRING(bookmark.getName()));
    }

    if (bookmark.getNick()) {
        nick_->setText(P2QSTRING((*bookmark.getNick())));
    }

    if (bookmark.getPassword()) {
        password_->setText(P2QSTRING((*bookmark.getPassword())));
    }
}

}
