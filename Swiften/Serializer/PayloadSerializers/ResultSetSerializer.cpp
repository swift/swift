/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

ResultSetSerializer::ResultSetSerializer() {
}

ResultSetSerializer::~ResultSetSerializer() {
}

std::string ResultSetSerializer::serializePayload(std::shared_ptr<ResultSet> payload) const {
    if (!payload) {
        return "";
    }

    XMLElement element("set", "http://jabber.org/protocol/rsm");

    if (payload->getMaxItems()) {
        element.addNode(std::make_shared<XMLElement>("max", "", std::to_string(*payload->getMaxItems())));
    }

    if (payload->getCount()) {
        element.addNode(std::make_shared<XMLElement>("count", "", std::to_string(*payload->getCount())));
    }

    if (payload->getIndex()) {
        element.addNode(std::make_shared<XMLElement>("index", "", std::to_string(*payload->getIndex())));
    }

    if (payload->getFirstID()) {
        std::shared_ptr<XMLElement> firstElement = std::make_shared<XMLElement>("first", "", *payload->getFirstID());
        if (payload->getFirstIDIndex()) {
            firstElement->setAttribute("index", std::to_string(*payload->getFirstIDIndex()));
        }
        element.addNode(firstElement);
    }

    if (payload->getLastID()) {
        element.addNode(std::make_shared<XMLElement>("last", "", *payload->getLastID()));
    }

    if (payload->getBefore()) {
        element.addNode(std::make_shared<XMLElement>("before", "", *payload->getBefore()));
    }

    if (payload->getAfter()) {
        element.addNode(std::make_shared<XMLElement>("after", "", *payload->getAfter()));
    }

    return element.serialize();
}
