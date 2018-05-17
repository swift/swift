/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEditBookmarkWindow.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtEditBookmarkWindow::QtEditBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark) : eventStream_(eventStream), bookmark_(bookmark) {
    name_->setText(P2QSTRING(bookmark.getName()));
    room_->setText(P2QSTRING(bookmark.getRoom().toString()));
    nick_->setText(bookmark.getNick() ? P2QSTRING(bookmark.getNick().get()) : "");
    password_->setText(bookmark.getPassword() ? P2QSTRING(bookmark.getPassword().get()) : "");
}

bool QtEditBookmarkWindow::commit() {
    boost::optional<MUCBookmark> bookmark = createBookmarkFromForm();
    if (!bookmark) {
        return false;
    }
    eventStream_->send(std::make_shared<EditMUCBookmarkUIEvent>(bookmark_, *bookmark));
    return true;
}

}

