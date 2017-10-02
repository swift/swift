/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace Swift {
    class TabComplete {
        public:
            void addWord(const std::string& word);
            void removeWord(const std::string& word);
            std::string completeWord(const std::string& word);
        private:
            std::vector<std::string> words_;
            std::string lastCompletion_;
            std::string lastShort_;
            std::vector<std::string> lastCompletionCandidates_;
    };
}
