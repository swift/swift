/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtEditBookmarkWindow.h"

namespace Swift {
QtEditBookmarkWindow::QtEditBookmarkWindow(UIEventStream* eventStream, boost::shared_ptr<MUCBookmark> bookmark) : eventStream_(eventStream), bookmark_(bookmark) {

}

void QtEditBookmarkWindow::commit() {
	eventStream_->send(boost::shared_ptr<UIEvent>(new EditMUCBookmarkUIEvent(bookmark_, createBookmarkFromForm())));
}

}

