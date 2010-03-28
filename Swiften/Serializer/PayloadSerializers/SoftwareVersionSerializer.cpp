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
