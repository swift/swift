/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swift/Controllers/ContactSuggester.h>
#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/RosterFilter.h>
#include <Swift/Controllers/Roster/RosterItem.h>

namespace Swift {

class FuzzyRosterFilter : public RosterFilter {
    public:
        FuzzyRosterFilter(const std::string& query) : query_(query) { }
        virtual ~FuzzyRosterFilter() {}
        virtual bool operator() (RosterItem* item) const {
            ContactRosterItem *contactItem = dynamic_cast<ContactRosterItem*>(item);
            if (contactItem) {
                const bool itemMatched = ContactSuggester::fuzzyMatch(contactItem->getDisplayName(), query_) || ContactSuggester::fuzzyMatch(contactItem->getDisplayJID(), query_);
                return !itemMatched;
            } else {
                return false;
            }
        }

    private:
        std::string query_;
};

}


