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

#pragma once

#include <string>
#include <vector>

#include <Swift/Controllers/Contact.h>

namespace Swift {
	class ContactProvider;

	class ContactSuggester {
	public:
		ContactSuggester();
		~ContactSuggester();

		void addContactProvider(ContactProvider* provider);

		std::vector<Contact::ref> getSuggestions(const std::string& search) const;
	private:
		static bool matchContact(const std::string& search, const Contact::ref& c);
		/**
		 * Performs fuzzy matching on the string text. Matches when each character of match string is present in sequence in text string.
		 */
		static bool fuzzyMatch(std::string text, std::string match);
		static bool contactLexicographicalSortPredicate(const Contact::ref& a, const Contact::ref& b);
		static bool contactEqualityPredicate(const Contact::ref& a, const Contact::ref& b);
		static bool contactSmartSortPredicate(const Contact::ref& a, const Contact::ref& b);

	private:
		std::vector<ContactProvider*> contactProviders_;
	};
}
