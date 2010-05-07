/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class AddMUCBookmarkUIEvent : public UIEvent {
		public:
			AddMUCBookmarkUIEvent(const MUCBookmark& bookmark) : bookmark(bookmark) {};
			const MUCBookmark& getBookmark() { return bookmark; }

		private:
			MUCBookmark bookmark;
	};
}
