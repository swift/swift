/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MAMResultSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MAMResultSerializer::MAMResultSerializer(PayloadSerializerCollection* serializers) : serializers_(serializers) {
}

MAMResultSerializer::~MAMResultSerializer() {
}

std::string MAMResultSerializer::serializePayload(std::shared_ptr<MAMResult> payload) const {
    if (!payload) {
        return "";
    }

    XMLElement element("result", "urn:xmpp:mam:0");

    element.setAttribute("id", payload->getID());

    if (payload->getQueryID()) {
        element.setAttribute("queryid", *payload->getQueryID());
    }

    element.addNode(std::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(payload->getPayload())));

    return element.serialize();
}
