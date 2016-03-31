/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace Swift {
typedef std::pair<std::string, int> UnreadPair;

    class ChatMessageSummarizer {
        public:
            std::string getSummary(const std::string& current, const std::vector<UnreadPair>& unreads);
    };
}
