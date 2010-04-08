/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h"

namespace Swift {

SoftwareVersionSerializer::SoftwareVersionSerializer() : GenericPayloadSerializer<SoftwareVersion>() {
}

String SoftwareVersionSerializer::serializePayload(boost::shared_ptr<SoftwareVersion> version)  const {
	String result("<query xmlns=\"jabber:iq:version\">");
	if (!version->getName().isEmpty()) {
		result += "<name>" + version->getName() + "</name>";
	}
	if (!version->getVersion().isEmpty()) {
		result += "<version>" + version->getVersion() + "</version>";
	}
	if (!version->getOS().isEmpty()) {
		result += "<os>" + version->getOS() + "</os>";
	}
	result += "</query>";
	return result;
}

}
