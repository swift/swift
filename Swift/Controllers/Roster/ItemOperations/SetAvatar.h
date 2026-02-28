/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Elements/Presence.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>

namespace Swift {

class RosterItem;

class SetAvatar : public RosterItemOperation {
    public:
        SetAvatar(const JID& jid, const boost::filesystem::path& path, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), jid_(jid), path_(path), compareType_(compareType) {
        }

        virtual void operator() (RosterItem* item) const {
            ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
            if (contact && contact->getJID().equals(jid_, compareType_)) {
                contact->setAvatarPath(path_);
            }
        }

    private:
        JID jid_;
        boost::filesystem::path path_;
        JID::CompareType compareType_;
};

}
