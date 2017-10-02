/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXParticipantSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MIXParticipantSerializer::MIXParticipantSerializer() : GenericPayloadSerializer<MIXParticipant>() {
}

std::string MIXParticipantSerializer::serializePayload(std::shared_ptr<MIXParticipant> payload)  const {
    XMLElement participantElement("participant", "urn:xmpp:mix:0");

    if (payload->getNick()) {
        std::shared_ptr<XMLElement> nickElement = std::make_shared<XMLElement>("nick");
        nickElement->addNode(std::make_shared<XMLTextNode>(*payload->getNick()));
        participantElement.addNode(nickElement);
    }

    if (payload->getJID()) {
        std::shared_ptr<XMLElement> jidElement = std::make_shared<XMLElement>("jid");
        jidElement->addNode(std::make_shared<XMLTextNode>(*payload->getJID()));
        participantElement.addNode(jidElement);
    }
    return participantElement.serialize();
}

}
