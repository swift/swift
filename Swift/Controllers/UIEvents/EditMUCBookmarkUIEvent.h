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
	class EditMUCBookmarkUIEvent : public UIEvent {
		public:
			EditMUCBookmarkUIEvent(const MUCBookmark& oldBookmark, const MUCBookmark& newBookmark) : oldBookmark(oldBookmark) , newBookmark(newBookmark) {}

			const MUCBookmark& getOldBookmark() {return oldBookmark;}
			const MUCBookmark& getNewBookmark() {return newBookmark;}

		private:
			MUCBookmark oldBookmark;
			MUCBookmark newBookmark;
	};
}
