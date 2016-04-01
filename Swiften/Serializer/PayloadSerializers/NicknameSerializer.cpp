/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/NicknameSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

NicknameSerializer::NicknameSerializer() : GenericPayloadSerializer<Nickname>() {
}

std::string NicknameSerializer::serializePayload(std::shared_ptr<Nickname> nick)  const {
    XMLElement nickElement("nick", "http://jabber.org/protocol/nick");
    nickElement.addNode(std::make_shared<XMLTextNode>(nick->getNickname()));
    return nickElement.serialize();
}

}
