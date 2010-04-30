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
	class EditMUCBookmarkUIEvent : public UIEvent {
		public:
			EditMUCBookmarkUIEvent(const boost::shared_ptr<MUCBookmark> oldBookmark, const boost::shared_ptr<MUCBookmark> newBookmark) : oldBookmark_(oldBookmark) , newBookmark_(newBookmark) {};
			boost::shared_ptr<MUCBookmark> getOldBookmark() {return oldBookmark_;};
			boost::shared_ptr<MUCBookmark> getNewBookmark() {return newBookmark_;};
		private:
			boost::shared_ptr<MUCBookmark> oldBookmark_;
			boost::shared_ptr<MUCBookmark> newBookmark_;
	};
}
