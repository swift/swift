/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <Swiften/JID/JID.h>
#include <Swiften/History/HistoryMessage.h>

namespace Swift {
	class HistoryManager {
		public:
			virtual ~HistoryManager();

			virtual void addMessage(const HistoryMessage& message) = 0;

			virtual std::vector<HistoryMessage> getMessages() const = 0;
	};
}
