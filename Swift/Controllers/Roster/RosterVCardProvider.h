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

#include <boost/signals2.hpp>
#include <boost/signals2/connection.hpp>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

namespace Swift {

class Roster;
class VCardManager;

class RosterVCardProvider {
    public:
        RosterVCardProvider(Roster* roster, VCardManager* vcardManager, JID::CompareType compareType);
        ~RosterVCardProvider();

    private:
        void handleVCardUpdateRequested(const JID& jid);
        void handleVCardChanged(const JID& jid, VCard::ref vcard);

    private:
        Roster* roster_;
        VCardManager* vcardManager_;
        JID::CompareType compareType_;
        boost::signals2::scoped_connection vcardUpdateRequestedConnection;
        boost::signals2::scoped_connection vcardChangedConnection;
};

}
