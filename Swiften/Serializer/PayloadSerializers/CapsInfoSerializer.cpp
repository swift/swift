/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

CapsInfoSerializer::CapsInfoSerializer() : GenericPayloadSerializer<CapsInfo>() {
}

std::string CapsInfoSerializer::serializePayload(std::shared_ptr<CapsInfo> capsInfo)  const {
    XMLElement capsElement("c", "http://jabber.org/protocol/caps");
    capsElement.setAttribute("node", capsInfo->getNode());
    capsElement.setAttribute("hash", capsInfo->getHash());
    capsElement.setAttribute("ver", capsInfo->getVersion());
    return capsElement.serialize();
}

}
