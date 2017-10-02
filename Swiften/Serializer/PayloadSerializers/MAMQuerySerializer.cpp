/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MAMQuerySerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MAMQuerySerializer::MAMQuerySerializer() {
}

MAMQuerySerializer::~MAMQuerySerializer() {
}

std::string MAMQuerySerializer::serializePayload(std::shared_ptr<MAMQuery> payload) const {
    if (!payload) {
        return "";
    }

    XMLElement element("query", "urn:xmpp:mam:0");

    if (payload->getQueryID()) {
        element.setAttribute("queryid", *payload->getQueryID());
    }

    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }

    if (payload->getForm()) {
        element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getForm())));
    }

    if (payload->getResultSet()) {
        element.addNode(std::make_shared<XMLRawTextNode>(ResultSetSerializer().serialize(payload->getResultSet())));
    }

    return element.serialize();
}
