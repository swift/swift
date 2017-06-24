/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API MUCOccupant {
        public:
            enum Role {Moderator, Participant, Visitor, NoRole};
            enum Affiliation {Owner, Admin, Member, Outcast, NoAffiliation};

            MUCOccupant(const std::string &nick, Role role, Affiliation affiliation);
            MUCOccupant(const MUCOccupant& other);
            ~MUCOccupant();

            MUCOccupant& operator=(MUCOccupant other);

            std::string getNick() const;
            Role getRole() const;
            Affiliation getAffiliation() const;
            boost::optional<JID> getRealJID() const;
            void setRealJID(const JID& jid);
            void setNick(const std::string& nick);

        private:
            std::string nick_;
            Role role_;
            Affiliation affiliation_;
            boost::optional<JID> realJID_;
            /* If you add a field, remember to update the const copy constructor */
    };
}

