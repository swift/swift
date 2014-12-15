/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include "QtBookmarkDetailWindow.h"

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"

namespace Swift {
	class QtAddBookmarkWindow : public QtBookmarkDetailWindow {
		Q_OBJECT
		public:
			QtAddBookmarkWindow(UIEventStream* eventStream);
			QtAddBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark);
			bool commit();
		private:
			UIEventStream* eventStream_;
	};
}
