/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
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

		std::vector<Contact> getSuggestions(const std::string& search) const;
	private:
		static bool matchContact(const std::string& search, const Contact& c);
		/**
		 * Performs fuzzy matching on the string text. Matches when each character of match string is present in sequence in text string.
		 */
		static bool fuzzyMatch(std::string text, std::string match);
		static bool chatSortPredicate(const Contact& a, const Contact& b);

	private:
		std::vector<ContactProvider*> contactProviders_;
	};
}
