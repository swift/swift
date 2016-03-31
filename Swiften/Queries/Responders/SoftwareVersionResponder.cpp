/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Queries/Responders/SoftwareVersionResponder.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/IQRouter.h>

namespace Swift {

SoftwareVersionResponder::SoftwareVersionResponder(IQRouter* router) : GetResponder<SoftwareVersion>(router) {
}

void SoftwareVersionResponder::setVersion(const std::string& client, const std::string& version, const std::string& os) {
    this->client = client;
    this->version = version;
    this->os = os;
}

bool SoftwareVersionResponder::handleGetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<SoftwareVersion>) {
    sendResponse(from, id, boost::make_shared<SoftwareVersion>(client, version, os));
    return true;
}

}
