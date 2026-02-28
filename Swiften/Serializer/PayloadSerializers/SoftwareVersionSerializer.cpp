/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.h>

namespace Swift {

SoftwareVersionSerializer::SoftwareVersionSerializer() : GenericPayloadSerializer<SoftwareVersion>() {
}

std::string SoftwareVersionSerializer::serializePayload(std::shared_ptr<SoftwareVersion> version)  const {
    std::string result("<query xmlns=\"jabber:iq:version\">");
    if (!version->getName().empty()) {
        result += "<name>" + version->getName() + "</name>";
    }
    if (!version->getVersion().empty()) {
        result += "<version>" + version->getVersion() + "</version>";
    }
    if (!version->getOS().empty()) {
        result += "<os>" + version->getOS() + "</os>";
    }
    result += "</query>";
    return result;
}

}
