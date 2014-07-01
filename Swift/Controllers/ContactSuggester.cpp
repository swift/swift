/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/ContactSuggester.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/bind.hpp>
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

bool ContactSuggester::matchContact(const std::string& search, const Contact::ref& c) {
	return fuzzyMatch(c->name, search) || fuzzyMatch(c->jid.toString(), search);
}

std::vector<Contact::ref> ContactSuggester::getSuggestions(const std::string& search) const {
	std::vector<Contact::ref> results;

	foreach(ContactProvider* provider, contactProviders_) {
		append(results, provider->getContacts());
	}

	std::sort(results.begin(), results.end(), Contact::lexicographicalSortPredicate);
	results.erase(std::unique(results.begin(), results.end(), Contact::equalityPredicate), results.end());
	results.erase(std::remove_if(results.begin(), results.end(), !lambda::bind(&matchContact, search, lambda::_1)),
		results.end());
	std::sort(results.begin(), results.end(), boost::bind(&Contact::sortPredicate, _1, _2, search));

	return results;
}

bool ContactSuggester::fuzzyMatch(std::string text, std::string match) {
	std::string lowerText = text;
	boost::algorithm::to_lower(lowerText);
	std::string lowerMatch = match;
	boost::algorithm::to_lower(lowerMatch);
	size_t lastMatch = 0;
	for (size_t i = 0; i < lowerMatch.length(); ++i) {
		size_t where = lowerText.find_first_of(lowerMatch[i], lastMatch);
		if (where == std::string::npos) {
			return false;
		}
		lastMatch = where + 1;
	}
	return true;
}

}
