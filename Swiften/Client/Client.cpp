/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) : CoreClient(jid, password) {
	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();
}

Client::~Client() {
	softwareVersionResponder->stop();
	delete softwareVersionResponder;
}

void Client::setSoftwareVersion(const String& name, const String& version) {
	softwareVersionResponder->setVersion(name, version);
}

}
