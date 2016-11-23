/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Roster/XMPPRosterController.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Roster/GetRosterRequest.h>
#include <Swiften/Roster/RosterStorage.h>
#include <Swiften/Roster/XMPPRosterImpl.h>

namespace Swift {

/**
 * The controller does not gain ownership of these parameters.
 */
XMPPRosterController::XMPPRosterController(IQRouter* iqRouter, XMPPRosterImpl* xmppRoster, RosterStorage* rosterStorage) : iqRouter_(iqRouter), rosterPushResponder_(iqRouter), xmppRoster_(xmppRoster), rosterStorage_(rosterStorage), useVersioning(false) {
    rosterPushResponder_.onRosterReceived.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1, false, std::shared_ptr<RosterPayload>()));
    rosterPushResponder_.start();
}

XMPPRosterController::~XMPPRosterController() {
    rosterPushResponder_.stop();
}

void XMPPRosterController::requestRoster() {
    xmppRoster_->clear();

    std::shared_ptr<RosterPayload> storedRoster = rosterStorage_->getRoster();
    GetRosterRequest::ref rosterRequest;
    if (useVersioning) {
        std::string version = "";
        if (storedRoster && storedRoster->getVersion()) {
            version = *storedRoster->getVersion();
        }
        rosterRequest = GetRosterRequest::create(iqRouter_, version);
    }
    else {
        rosterRequest = GetRosterRequest::create(iqRouter_);
    }
    rosterRequest->onResponse.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1, true, storedRoster));
    rosterRequest->send();
}

void XMPPRosterController::handleRosterReceived(std::shared_ptr<RosterPayload> rosterPayload, bool initial, std::shared_ptr<RosterPayload> previousRoster) {
    if (rosterPayload) {
        for (const auto& item : rosterPayload->getItems()) {
            //Don't worry about the updated case, the XMPPRoster sorts that out.
            if (item.getSubscription() == RosterItemPayload::Remove) {
                xmppRoster_->removeContact(item.getJID());
            } else {
                xmppRoster_->addContact(item.getJID(), item.getName(), item.getGroups(), item.getSubscription());
            }
        }
    }
    else if (previousRoster) {
        // The cached version hasn't changed; emit all items
        for (const auto& item : previousRoster->getItems()) {
            if (item.getSubscription() != RosterItemPayload::Remove) {
                xmppRoster_->addContact(item.getJID(), item.getName(), item.getGroups(), item.getSubscription());
            }
            else {
                SWIFT_LOG(error) << "Stored invalid roster item" << std::endl;
            }
        }
    }
    if (initial) {
        xmppRoster_->onInitialRosterPopulated();
    }
    if (rosterPayload && rosterPayload->getVersion() && useVersioning) {
        saveRoster(*rosterPayload->getVersion());
    }
}

void XMPPRosterController::saveRoster(const std::string& version) {
    std::vector<XMPPRosterItem> items = xmppRoster_->getItems();
    std::shared_ptr<RosterPayload> roster(new RosterPayload());
    roster->setVersion(version);
    for (const auto& item : items) {
        roster->addItem(RosterItemPayload(item.getJID(), item.getName(), item.getSubscription(), item.getGroups()));
    }
    rosterStorage_->setRoster(roster);
}

}
