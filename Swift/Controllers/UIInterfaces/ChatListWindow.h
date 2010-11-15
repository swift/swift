/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class ChatListWindow {
		public:
			virtual ~ChatListWindow();

			virtual void setBookmarksEnabled(bool enabled) = 0;
			virtual void addMUCBookmark(const MUCBookmark& bookmark) = 0;
			virtual void removeMUCBookmark(const MUCBookmark& bookmark) = 0;
			virtual void clear() = 0;
	};
}
