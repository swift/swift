#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class ChatListWindow {
		public:
			virtual void addMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) = 0;
			virtual void removeMUCBookmark(boost::shared_ptr<MUCBookmark> bookmark) = 0;
	};
}
