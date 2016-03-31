/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/HunspellChecker.h>

#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <hunspell/hunspell.hxx>

namespace Swift {

HunspellChecker::HunspellChecker(const char* affix_path, const char* dictionary_path) {
    speller_ = new Hunspell(affix_path, dictionary_path);
}

HunspellChecker::~HunspellChecker() {
    delete speller_;
}

bool HunspellChecker::isCorrect(const std::string& word) {
    return speller_->spell(word.c_str());
}

void HunspellChecker::getSuggestions(const std::string& word, std::vector<std::string>& list) {
    char **suggestList = NULL;
    int words_returned = 0;
    if (!word.empty()) {
        words_returned = speller_->suggest(&suggestList, word.c_str());
        if (suggestList != NULL) {
            for (int i = 0; i < words_returned; ++i) {
                list.push_back(suggestList[i]);
                free(suggestList[i]);
            }
            free(suggestList);
        }
    }
}

void HunspellChecker::checkFragment(const std::string& fragment, PositionPairList& misspelledPositions) {
    if (!fragment.empty()) {
        parser_->check(fragment, misspelledPositions);
        for (PositionPairList::iterator it = misspelledPositions.begin(); it != misspelledPositions.end();) {
            if (isCorrect(fragment.substr(boost::get<0>(*it), boost::get<1>(*it) - boost::get<0>(*it)))) {
                it = misspelledPositions.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

}
