/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <string>

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
