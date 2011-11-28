/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <SwifTools/SpellChecker.h>

#pragma once

class Hunspell;

namespace Swift {
	class HunspellChecker : public SpellChecker {
		public:
			HunspellChecker(const char* affix_path, const char* dict_path);
			virtual ~HunspellChecker();
			virtual bool isCorrect(const std::string& word);
			virtual void getSuggestions(const std::string& word, std::vector<std::string>& list);
			virtual void checkFragment(const std::string& fragment, PositionPairList& misspelledPositions);
		private:
			Hunspell* speller_;
	};
}
