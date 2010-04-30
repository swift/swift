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

void QtAddBookmarkWindow::commit() {
	qDebug() << "committing";
	eventStream_->send(boost::shared_ptr<UIEvent>(new AddMUCBookmarkUIEvent(createBookmarkFromForm())));
}

}
