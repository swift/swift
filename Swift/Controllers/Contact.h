/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {

class Contact : public std::enable_shared_from_this<Contact> {
    public:
        typedef std::shared_ptr<Contact> ref;

        Contact();
        Contact(const std::string& name, const JID& jid, StatusShow::Type statusType, const boost::filesystem::path& path);

        static bool lexicographicalSortPredicate(const Contact::ref& a, const Contact::ref& b);
        static bool equalityPredicate(const Contact::ref& a, const Contact::ref& b);
        static bool sortPredicate(const Contact::ref& a, const Contact::ref& b, const std::string& search);

    public:
        std::string name;
        JID jid;
        StatusShow::Type statusType;
        boost::filesystem::path avatarPath;
};

}
