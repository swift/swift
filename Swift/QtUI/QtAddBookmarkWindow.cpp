/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtAddBookmarkWindow.h>

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
        eventStream_->send(std::make_shared<AddMUCBookmarkUIEvent>(*bookmark));
        return true;
    }
    else {
        return false;
    }
}

}
