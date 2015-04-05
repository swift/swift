/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtAddBookmarkWindow.h"

namespace Swift {
QtAddBookmarkWindow::QtAddBookmarkWindow(UIEventStream* eventStream) : eventStream_(eventStream) {
	setWindowTitle(tr("Add Bookmark Details"));
}

QtAddBookmarkWindow::QtAddBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark) : eventStream_(eventStream) {
	createFormFromBookmark(bookmark);
	setWindowTitle(tr("Add Bookmark Details"));
}

bool QtAddBookmarkWindow::commit() {
	boost::optional<MUCBookmark> bookmark = createBookmarkFromForm();
	if (bookmark) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new AddMUCBookmarkUIEvent(*bookmark)));
		return true;
	}
	else {
		return false;
	}
}

}
