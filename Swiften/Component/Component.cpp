/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Component/Component.h"

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"

namespace Swift {

Component::Component(EventLoop* eventLoop, const JID& jid, const String& secret) : CoreComponent(eventLoop, jid, secret) {
	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();
}

Component::~Component() {
	softwareVersionResponder->stop();
	delete softwareVersionResponder;
}

void Component::setSoftwareVersion(const String& name, const String& version) {
	softwareVersionResponder->setVersion(name, version);
}

}
