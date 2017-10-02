/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventRedirectSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubEventRedirectSerializer::PubSubEventRedirectSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubEventRedirectSerializer::~PubSubEventRedirectSerializer() {
}

std::string PubSubEventRedirectSerializer::serializePayload(std::shared_ptr<PubSubEventRedirect> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("redirect", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("uri", payload->getURI());
    return element.serialize();
}


