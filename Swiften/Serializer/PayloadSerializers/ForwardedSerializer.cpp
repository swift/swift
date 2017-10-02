/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Serializer/GenericStanzaSerializer.h>
#include <Swiften/Serializer/IQSerializer.h>
#include <Swiften/Serializer/MessageSerializer.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/DelaySerializer.h>
#include <Swiften/Serializer/PresenceSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

ForwardedSerializer::ForwardedSerializer(PayloadSerializerCollection* serializers) : serializers_(serializers) {
}

ForwardedSerializer::~ForwardedSerializer() {
}

std::string ForwardedSerializer::serializePayload(std::shared_ptr<Forwarded> payload) const {
    if (!payload) {
        return "";
    }

    XMLElement element("forwarded", "urn:xmpp:forward:0");

    if (payload->getDelay()) {
        element.addNode(std::make_shared<XMLRawTextNode>(DelaySerializer().serialize(payload->getDelay())));
    }

    if (payload->getStanza()) { /* find out what type of stanza we are dealing with and branch into the correct serializer*/
        std::shared_ptr<IQ> iq;
        std::shared_ptr<Message> message;
        std::shared_ptr<Presence> presence;
        const std::string ns = "jabber:client";
        if ((iq = std::dynamic_pointer_cast<IQ>(payload->getStanza()))) {
            element.addNode(std::make_shared<XMLRawTextNode>(safeByteArrayToString(IQSerializer(serializers_).serialize(iq, ns))));
        } else if ((message = std::dynamic_pointer_cast<Message>(payload->getStanza()))) {
            element.addNode(std::make_shared<XMLRawTextNode>(safeByteArrayToString(MessageSerializer(serializers_).serialize(message, ns))));
        } else if ((presence = std::dynamic_pointer_cast<Presence>(payload->getStanza()))) {
            element.addNode(std::make_shared<XMLRawTextNode>(safeByteArrayToString(PresenceSerializer(serializers_).serialize(presence, ns))));
        }
    }

    return element.serialize();
}
