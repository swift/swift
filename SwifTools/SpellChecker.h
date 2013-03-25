/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <SwifTools/SpellParser.h>

#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>

#pragma once

namespace Swift {
	class SpellChecker {
		public:
			SpellChecker() {
				parser_ = new SpellParser();
			}
			virtual ~SpellChecker() {
				delete parser_;
			}
			virtual bool isCorrect(const std::string& word) = 0;
			virtual void getSuggestions(const std::string& word, std::vector<std::string>& list) = 0;
			virtual void checkFragment(const std::string& fragment, PositionPairList& misspelledPositions) = 0;
		protected:
			SpellParser *parser_;
	};
}
