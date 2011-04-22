/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
