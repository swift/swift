/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/TabComplete.h"
#include "Swiften/Base/foreach.h"

namespace Swift {

void TabComplete::addWord(const String& word) {
	words_.push_back(word);
	if (word.getLowerCase().beginsWith(lastShort_)) {
		lastCompletionCandidates_.push_back(word);
	}
}

void TabComplete::removeWord(const String& word) {
	words_.erase(std::remove(words_.begin(), words_.end(), word), words_.end());
	lastCompletionCandidates_.erase(std::remove(lastCompletionCandidates_.begin(), lastCompletionCandidates_.end(), word), lastCompletionCandidates_.end());
}

String TabComplete::completeWord(const String& word) {
	if (word == lastCompletion_) {
		if (lastCompletionCandidates_.size() != 0) {
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
		lastShort_ = word.getLowerCase();
		lastCompletionCandidates_.clear();
		foreach (String candidate, words_) {
			if (candidate.getLowerCase().beginsWith(word.getLowerCase())) {
				lastCompletionCandidates_.push_back(candidate);
			}
		}
		lastCompletion_ = lastCompletionCandidates_.size() > 0 ? lastCompletionCandidates_[0] : word;
	}
	return lastCompletion_;
}

}
