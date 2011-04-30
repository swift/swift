/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Roster/XMPPRosterController.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Roster/GetRosterRequest.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/Roster/RosterStorage.h>

namespace Swift {
	
/**
 * The controller does not gain ownership of these parameters.
 */
XMPPRosterController::XMPPRosterController(IQRouter* iqRouter, XMPPRosterImpl* xmppRoster, RosterStorage* rosterStorage) : iqRouter_(iqRouter), rosterPushResponder_(iqRouter), xmppRoster_(xmppRoster), rosterStorage_(rosterStorage), useVersioning(false) {
	rosterPushResponder_.onRosterReceived.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1, false, boost::shared_ptr<RosterPayload>()));
	rosterPushResponder_.start();
}

XMPPRosterController::~XMPPRosterController() {
	rosterPushResponder_.stop();
}

void XMPPRosterController::requestRoster() {
	xmppRoster_->clear();

	boost::shared_ptr<RosterPayload> storedRoster = rosterStorage_->getRoster();
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

void XMPPRosterController::handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload, bool initial, boost::shared_ptr<RosterPayload> previousRoster) {
	if (rosterPayload) {
		foreach(const RosterItemPayload& item, rosterPayload->getItems()) {
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
		foreach(const RosterItemPayload& item, previousRoster->getItems()) {
			if (item.getSubscription() != RosterItemPayload::Remove) {
				xmppRoster_->addContact(item.getJID(), item.getName(), item.getGroups(), item.getSubscription());
			}
			else {
				std::cerr << "ERROR: Stored invalid roster item" << std::endl;
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
	boost::shared_ptr<RosterPayload> roster(new RosterPayload());
	roster->setVersion(version);
	foreach(const XMPPRosterItem& item, items) {
		roster->addItem(RosterItemPayload(item.getJID(), item.getName(), item.getSubscription(), item.getGroups()));
	}
	rosterStorage_->setRoster(roster);
}

}
