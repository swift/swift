/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
