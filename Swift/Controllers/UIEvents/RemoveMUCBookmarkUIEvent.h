/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class RemoveMUCBookmarkUIEvent : public UIEvent {
		public:
			RemoveMUCBookmarkUIEvent(const MUCBookmark& bookmark) : bookmark(bookmark) {}
			const MUCBookmark& getBookmark() { return bookmark; }

		private:
			MUCBookmark bookmark;
	};
}
