/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once
#include "QtBookmarkDetailWindow.h"

#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h"

namespace Swift {
	class QtEditBookmarkWindow : public QtBookmarkDetailWindow {
		Q_OBJECT
		public:
			QtEditBookmarkWindow(UIEventStream* eventStream, const MUCBookmark& bookmark);
			bool commit();
	
		private:
			UIEventStream* eventStream_;
			MUCBookmark bookmark_;
	};
}
