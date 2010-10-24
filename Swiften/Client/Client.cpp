/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Roster/XMPPRosterImpl.h"
#include "Swiften/Roster/XMPPRosterController.h"
#include "Swiften/Presence/PresenceOracle.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) : CoreClient(jid, password) {
	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();

	roster = new XMPPRosterImpl();
	rosterController = new XMPPRosterController(getIQRouter(), roster);

	presenceOracle = new PresenceOracle(getStanzaChannel());
	presenceOracle->onPresenceChange.connect(boost::ref(onPresenceChange));
	presenceOracle->onPresenceSubscriptionRequest.connect(boost::ref(onPresenceSubscriptionRequest));
}

Client::~Client() {
	delete presenceOracle;

	delete rosterController;
	delete roster;

	softwareVersionResponder->stop();
	delete softwareVersionResponder;
}

XMPPRoster* Client::getRoster() const {
	return roster;
}

void Client::setSoftwareVersion(const String& name, const String& version) {
	softwareVersionResponder->setVersion(name, version);
}

void Client::requestRoster() {
	rosterController->requestRoster();
}


Presence::ref Client::getLastPresence(const JID& jid) const {
	return presenceOracle->getLastPresence(jid);
}

Presence::ref Client::getHighestPriorityPresence(const JID& bareJID) const {
	return presenceOracle->getHighestPriorityPresence(bareJID);
}

}
