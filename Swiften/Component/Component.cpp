/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/Component.h>

#include <Swiften/Queries/Responders/SoftwareVersionResponder.h>

namespace Swift {

Component::Component(EventLoop* eventLoop, NetworkFactories* networkFactories, const JID& jid, const std::string& secret) : CoreComponent(eventLoop, networkFactories, jid, secret) {
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
