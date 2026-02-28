/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swift/Controllers/Contact.h>

class ContactSuggesterTest;

namespace Swift {
    class ContactProvider;

    class ContactSuggester {
    public:
        ContactSuggester();
        ~ContactSuggester();

        void addContactProvider(ContactProvider* provider);

        std::vector<Contact::ref> getSuggestions(const std::string& search, bool withMUCNicks) const;
    public:
        static bool matchContact(const std::string& search, const Contact::ref& c);
        /**
         * Performs fuzzy matching on the string text. Matches when each character of match string is present in sequence in text string.
         */
        static bool fuzzyMatch(std::string text, std::string match);

    private:
        std::vector<ContactProvider*> contactProviders_;
    };
}
