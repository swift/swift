/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Queries/IQRouter.h"

namespace Swift {

SoftwareVersionResponder::SoftwareVersionResponder(
		const String& client, const String& version, IQRouter* router) : 
			GetResponder<SoftwareVersion>(router),  client_(client), version_(version) {
}

bool SoftwareVersionResponder::handleGetRequest(const JID& from, const String& id, boost::shared_ptr<SoftwareVersion>) {
	sendResponse(from, id, boost::shared_ptr<SoftwareVersion>(new SoftwareVersion(client_, version_)));
	return true;
}

}
