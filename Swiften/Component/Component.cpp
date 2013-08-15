/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/Component.h>

#include <Swiften/Queries/Responders/SoftwareVersionResponder.h>

namespace Swift {

Component::Component(const JID& jid, const std::string& secret, NetworkFactories* networkFactories) : CoreComponent(jid, secret, networkFactories) {
	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();
}

Component::~Component() {
	softwareVersionResponder->stop();
	delete softwareVersionResponder;
}

void Component::setSoftwareVersion(const std::string& name, const std::string& version) {
	softwareVersionResponder->setVersion(name, version);
}

}
