/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/Controllers/ContactSuggester.h>

#include <boost/algorithm/string/find.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/ContactProvider.h>

#include <algorithm>
#include <vector>
#include <set>

namespace lambda = boost::lambda;

namespace Swift {

ContactSuggester::ContactSuggester() {
}

ContactSuggester::~ContactSuggester() {
}

void ContactSuggester::addContactProvider(ContactProvider* provider) {
	contactProviders_.push_back(provider);
}

bool ContactSuggester::matchContact(const std::string& search, const Contact& c) {
	return fuzzyMatch(c.name, search) || fuzzyMatch(c.jid.toString(), search);
}

std::vector<Contact> ContactSuggester::getSuggestions(const std::string& search) const {
	std::vector<Contact> results;

	foreach(ContactProvider* provider, contactProviders_) {
		append(results, provider->getContacts());
	}

	std::sort(results.begin(), results.end(),
		lambda::bind(&Contact::jid, lambda::_1) < lambda::bind(&Contact::jid, lambda::_2));
	results.erase(std::unique(results.begin(), results.end(),
		lambda::bind(&Contact::jid, lambda::_1) == lambda::bind(&Contact::jid, lambda::_2)),
		results.end());
	results.erase(std::remove_if(results.begin(), results.end(), !lambda::bind(&ContactSuggester::matchContact, search, lambda::_1)),
		results.end());
	std::sort(results.begin(), results.end(), ContactSuggester::chatSortPredicate);

	return results;
}

bool ContactSuggester::fuzzyMatch(std::string text, std::string match) {
	for (std::string::iterator currentQueryChar = match.begin(); currentQueryChar != match.end(); currentQueryChar++) {
		//size_t result = text.find(*currentQueryChar);
		std::string::iterator result = boost::algorithm::ifind_first(text, std::string(currentQueryChar, currentQueryChar+1)).begin();
		if (result == text.end()) {
			return false;
		}
		text.erase(result);
	}
	return true;
}

bool ContactSuggester::chatSortPredicate(const Contact& a, const Contact& b) {
	if (a.statusType == b.statusType) {
		return a.name.compare(b.name) < 0;
	} else {
		return a.statusType < b.statusType;
	}
}

}
