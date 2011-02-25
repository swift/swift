/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/XMPPRosterController.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/GetRosterRequest.h"
#include "Swiften/Roster/XMPPRosterImpl.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of these parameters.
 */
XMPPRosterController::XMPPRosterController(IQRouter* iqRouter, XMPPRosterImpl* xmppRoster) : iqRouter_(iqRouter), rosterPushResponder_(iqRouter), xmppRoster_(xmppRoster) {
	rosterPushResponder_.onRosterReceived.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1, false));
	rosterPushResponder_.start();
}

XMPPRosterController::~XMPPRosterController() {
	rosterPushResponder_.stop();
}

void XMPPRosterController::requestRoster() {
	xmppRoster_->clear();
	GetRosterRequest::ref rosterRequest = GetRosterRequest::create(iqRouter_);
	rosterRequest->onResponse.connect(boost::bind(&XMPPRosterController::handleRosterReceived, this, _1, true));
	rosterRequest->send();
}

void XMPPRosterController::handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload, bool initial) {
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
	if (initial) {
		xmppRoster_->onInitialRosterPopulated();
	}
}

}
