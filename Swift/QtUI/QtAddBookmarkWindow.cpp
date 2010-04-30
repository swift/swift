/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtAddBookmarkWindow.h"

#include <qdebug.h>

namespace Swift {
QtAddBookmarkWindow::QtAddBookmarkWindow(UIEventStream* eventStream) : eventStream_(eventStream) {

}

bool QtAddBookmarkWindow::commit() {
	boost::shared_ptr<MUCBookmark> bookmark(createBookmarkFromForm());
	if (bookmark) {
		eventStream_->send(boost::shared_ptr<UIEvent>(new AddMUCBookmarkUIEvent(bookmark)));
	}
	return bookmark;
}

}
