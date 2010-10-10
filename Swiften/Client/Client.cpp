/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/Roster/XMPPRosterController.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) : CoreClient(jid, password) {
	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();

	roster = new XMPPRoster();
	rosterController = new XMPPRosterController(getIQRouter(), roster);
}

Client::~Client() {
	delete rosterController;
	delete roster;

	softwareVersionResponder->stop();
	delete softwareVersionResponder;
}

void Client::setSoftwareVersion(const String& name, const String& version) {
	softwareVersionResponder->setVersion(name, version);
}

void Client::requestRoster() {
	rosterController->requestRoster();
}


}
