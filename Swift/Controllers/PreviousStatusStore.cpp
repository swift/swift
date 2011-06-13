/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "PreviousStatusStore.h"

#include "Swiften/Base/foreach.h"

namespace Swift {

PreviousStatusStore::PreviousStatusStore() {

}

PreviousStatusStore::~PreviousStatusStore() {

}

void PreviousStatusStore::addStatus(StatusShow::Type status, const std::string& message) {
	//FIXME: remove old entries
	store_.push_back(TypeStringPair(status, message));
}

std::vector<TypeStringPair> PreviousStatusStore::exactMatchSuggestions(StatusShow::Type status, const std::string& message) {
	std::vector<TypeStringPair> suggestions;
	suggestions.push_back(TypeStringPair(status, message));
	return suggestions;
}

std::vector<TypeStringPair> PreviousStatusStore::getSuggestions(const std::string& message) {
	std::vector<TypeStringPair> suggestions;
	foreach (TypeStringPair status, store_) {
		if (status.second == message) {
			suggestions.clear();
			suggestions.push_back(status);
			break;
		} else if (status.second.find(message) != std::string::npos) {
			suggestions.push_back(status);
		}
	}
	if (suggestions.empty()) {
		TypeStringPair suggestion(StatusShow::Online, message);
		suggestions.push_back(suggestion);
	}
	if (suggestions.size() == 1) {
		suggestions = exactMatchSuggestions(suggestions[0].first, suggestions[0].second);
	}
	return suggestions;
}

}
