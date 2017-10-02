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

#include <Swift/Controllers/Roster/RosterVCardProvider.h>

#include <Swiften/VCards/VCardManager.h>

#include <Swift/Controllers/Roster/ItemOperations/SetVCard.h>
#include <Swift/Controllers/Roster/Roster.h>

namespace Swift {

RosterVCardProvider::RosterVCardProvider(Roster* roster, VCardManager* vcardManager, JID::CompareType compareType) : roster_(roster), vcardManager_(vcardManager), compareType_(compareType) {
    vcardUpdateRequestedConnection = roster_->onVCardUpdateRequested.connect(boost::bind(&RosterVCardProvider::handleVCardUpdateRequested, this, _1));
    vcardChangedConnection = vcardManager_->onVCardChanged.connect(boost::bind(&RosterVCardProvider::handleVCardChanged, this, _1, _2));
}

RosterVCardProvider::~RosterVCardProvider() {
}

void RosterVCardProvider::handleVCardUpdateRequested(const JID& jid) {
    VCard::ref vcard = vcardManager_->getVCardAndRequestWhenNeeded(jid);
    if (vcard) {
        handleVCardChanged(jid, vcard);
    }
}

void RosterVCardProvider::handleVCardChanged(const JID& jid, VCard::ref vcard) {
    roster_->applyOnItem(SetVCard(jid, vcard, compareType_), jid);
}


}
