/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/MessageSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

MessageSerializer::MessageSerializer(PayloadSerializerCollection* payloadSerializers, const boost::optional<std::string>& explicitNS) :
        GenericStanzaSerializer<Message>("message", payloadSerializers, explicitNS) {
}

void MessageSerializer::setStanzaSpecificAttributesGeneric(
        std::shared_ptr<Message> message,
        XMLElement& element) const {
    if (message->getType() == Message::Chat) {
        element.setAttribute("type", "chat");
    }
    else if (message->getType() == Message::Groupchat) {
        element.setAttribute("type", "groupchat");
    }
    else if (message->getType() == Message::Headline) {
        element.setAttribute("type", "headline");
    }
    else if (message->getType() == Message::Error) {
        element.setAttribute("type", "error");
    }
}

}
