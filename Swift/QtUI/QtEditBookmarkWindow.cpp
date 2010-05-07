/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtEditBookmarkWindow.h"

#include "QtSwiftUtil.h"

namespace Swift {
QtEditBookmarkWindow::QtEditBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark) : eventStream_(eventStream), bookmark_(bookmark) {
	name_->setText(P2QSTRING(bookmark.getName()));
	room_->setText(P2QSTRING(bookmark.getRoom().toString()));
	autojoin_->setChecked(bookmark.getAutojoin());
	nick_->setText(bookmark.getNick() ? P2QSTRING(bookmark.getNick().get()) : "");
	password_->setText(bookmark.getPassword() ? P2QSTRING(bookmark.getPassword().get()) : "");
}

bool QtEditBookmarkWindow::commit() {
	boost::optional<MUCBookmark> bookmark = createBookmarkFromForm();
	if (!bookmark) {
		return false;
	}
	eventStream_->send(boost::shared_ptr<UIEvent>(new EditMUCBookmarkUIEvent(bookmark_, *bookmark)));
	return true;
}

}

