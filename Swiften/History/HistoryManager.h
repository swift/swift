#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/History/HistoryMessage.h"

namespace Swift {
	class HistoryManager {
		public:
			virtual ~HistoryManager();

			virtual void addMessage(const HistoryMessage& message) = 0;

			virtual std::vector<HistoryMessage> getMessages() const = 0;
	};
}
