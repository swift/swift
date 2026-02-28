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

#include <Swift/Controllers/Contact.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>

namespace Swift {

Contact::Contact() : statusType(StatusShow::None) {
}

Contact::Contact(const std::string& name, const JID& jid, StatusShow::Type statusType, const boost::filesystem::path& path) : name(name), jid(jid), statusType(statusType), avatarPath(path) {
}

bool Contact::lexicographicalSortPredicate(const Contact::ref& a, const Contact::ref& b) {
    if (a->jid.isValid() && b->jid.isValid()) {
        return a->jid < b->jid;
    } else {
        return a->name < b->name;
    }
}

bool Contact::equalityPredicate(const Contact::ref& a, const Contact::ref& b) {
    if (a->jid.isValid() && b->jid.isValid()) {
        return a->jid == b->jid;
    } else {
        return a->name == b->name;
    }
}

bool Contact::sortPredicate(const Contact::ref& a, const Contact::ref& b, const std::string& search) {
    /* perform case insensitive comparisons */
    std::string aLower = a->name;
    boost::to_lower(aLower);
    std::string bLower = b->name;
    boost::to_lower(bLower);
    std::string searchLower = search;
    boost::to_lower(searchLower);

    /* name starts with the search term */
    if (aLower.find(searchLower) == 0 && bLower.find(searchLower) != 0) {
        return true;
    } else if (bLower.find(searchLower) == 0 && aLower.find(searchLower) != 0) {
        return false;
    }

    /* name contains search term */
    if (aLower.find(searchLower) != std::string::npos && bLower.find(searchLower) == std::string::npos) {
        return true;
    } else if (bLower.find(searchLower) != std::string::npos && aLower.find(searchLower) == std::string::npos) {
        return false;
    }

    /* Levenshtein should be done here */
    /* if edit distances are equal, fall through to the tests below */

    /* lexicographical sort */
    if (a->statusType == b->statusType) {
        return aLower.compare(bLower) < 0;
    }

    /* online status */
    return a->statusType < b->statusType;
}

}
