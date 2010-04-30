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
	class RemoveMUCBookmarkUIEvent : public UIEvent {
		public:
			RemoveMUCBookmarkUIEvent(const boost::shared_ptr<MUCBookmark> bookmark) : bookmark_(bookmark) {};
			boost::shared_ptr<MUCBookmark> getBookmark() {return bookmark_;};
		private:
			boost::shared_ptr<MUCBookmark> bookmark_;
	};
}
