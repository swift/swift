/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include "Swiften/Base/String.h"

namespace Swift {
	class TabComplete {
		public:
			void addWord(const String& word);
			void removeWord(const String& word);
			String completeWord(const String& word);
		private:
			std::vector<String> words_;
			String lastCompletion_;
			String lastShort_;
			std::vector<String> lastCompletionCandidates_;
	};
}
