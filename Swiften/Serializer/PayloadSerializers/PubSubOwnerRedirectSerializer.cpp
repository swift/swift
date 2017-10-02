/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerRedirectSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubOwnerRedirectSerializer::PubSubOwnerRedirectSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubOwnerRedirectSerializer::~PubSubOwnerRedirectSerializer() {
}

std::string PubSubOwnerRedirectSerializer::serializePayload(std::shared_ptr<PubSubOwnerRedirect> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("redirect", "http://jabber.org/protocol/pubsub#owner");
    element.setAttribute("uri", payload->getURI());
    return element.serialize();
}
