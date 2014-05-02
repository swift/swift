/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtAddBookmarkWindow.h"

#include <qdebug.h>

namespace Swift {
QtAddBookmarkWindow::QtAddBookmarkWindow(UIEventStream* eventStream) : eventStream_(eventStream) {

}

QtAddBookmarkWindow::QtAddBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark) : eventStream_(eventStream) {
	createFormFromBookmark(bookmark);
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
