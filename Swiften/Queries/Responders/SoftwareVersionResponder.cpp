/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Queries/IQRouter.h"

namespace Swift {

SoftwareVersionResponder::SoftwareVersionResponder(IQRouter* router) : GetResponder<SoftwareVersion>(router) {
}

void SoftwareVersionResponder::setVersion(const std::string& client, const std::string& version, const std::string& os) {
	this->client = client;
	this->version = version;
	this->os = os;
}

bool SoftwareVersionResponder::handleGetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<SoftwareVersion>) {
	sendResponse(from, id, boost::shared_ptr<SoftwareVersion>(new SoftwareVersion(client, version, os)));
	return true;
}

}
