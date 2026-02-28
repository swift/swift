/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXRegisterNickSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MIXRegisterNickSerializer::MIXRegisterNickSerializer() : GenericPayloadSerializer<MIXRegisterNick>() {
}

std::string MIXRegisterNickSerializer::serializePayload(std::shared_ptr<MIXRegisterNick> payload)  const {
    XMLElement registernickElement("register", "urn:xmpp:mix:0");

    auto nickElement = std::make_shared<XMLElement>("nick");
    nickElement->addNode(std::make_shared<XMLTextNode>(payload->getNick()));
    registernickElement.addNode(nickElement);

    return registernickElement.serialize();
}

}
