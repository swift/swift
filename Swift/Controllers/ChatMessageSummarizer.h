/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <utility>
#include <string>

namespace Swift {
typedef std::pair<std::string, int> UnreadPair;

	class ChatMessageSummarizer {
		public:
			std::string getSummary(const std::string& current, const std::vector<UnreadPair>& unreads);
	};
}
