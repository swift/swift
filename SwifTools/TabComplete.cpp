/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/TabComplete.h>

#include <algorithm>

#include <boost/algorithm/string.hpp>

namespace Swift {

void TabComplete::addWord(const std::string& word) {
    words_.erase(std::remove(words_.begin(), words_.end(), word), words_.end());
    words_.insert(words_.begin(), word);
    if (boost::starts_with(boost::to_lower_copy(word), lastShort_)) {
        lastCompletionCandidates_.insert(lastCompletionCandidates_.begin(), word);
    }
}

void TabComplete::removeWord(const std::string& word) {
    words_.erase(std::remove(words_.begin(), words_.end(), word), words_.end());
    lastCompletionCandidates_.erase(std::remove(lastCompletionCandidates_.begin(), lastCompletionCandidates_.end(), word), lastCompletionCandidates_.end());
}

std::string TabComplete::completeWord(const std::string& word) {
    if (word == lastCompletion_) {
        if (!lastCompletionCandidates_.empty()) {
            size_t match = 0;
            for (match = 0; match < lastCompletionCandidates_.size(); match++) {
                if (lastCompletionCandidates_[match] == lastCompletion_) {
                    break;
                }
            }
            size_t nextIndex = match + 1;
            nextIndex = nextIndex >= lastCompletionCandidates_.size() ? 0 : nextIndex;
            lastCompletion_ = lastCompletionCandidates_[nextIndex];
        }
    } else {
        lastShort_ = boost::to_lower_copy(word);
        lastCompletionCandidates_.clear();
        for (auto&& candidate : words_) {
            if (boost::starts_with(boost::to_lower_copy(candidate), boost::to_lower_copy(word))) {
                lastCompletionCandidates_.push_back(candidate);
            }
        }
        lastCompletion_ = !lastCompletionCandidates_.empty() ? lastCompletionCandidates_[0] : word;
    }
    return lastCompletion_;
}

}
