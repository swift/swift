/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

SecurityLabelsCatalogSerializer::SecurityLabelsCatalogSerializer() : GenericPayloadSerializer<SecurityLabelsCatalog>() {
}

std::string SecurityLabelsCatalogSerializer::serializePayload(std::shared_ptr<SecurityLabelsCatalog> catalog)  const {
    XMLElement element("catalog", "urn:xmpp:sec-label:catalog:2");
    if (!catalog->getName().empty()) {
        element.setAttribute("name", catalog->getName());
    }
    if (catalog->getTo().isValid()) {
        element.setAttribute("to", catalog->getTo());
    }
    if (!catalog->getDescription().empty()) {
        element.setAttribute("desc", catalog->getDescription());
    }
    for (const auto& item : catalog->getItems()) {
        std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
        itemElement->setAttribute("selector", item.getSelector());
        if (item.getIsDefault()) {
            itemElement->setAttribute("default", "true");
        }
        if (item.getLabel()) {
            std::string serializedLabel = SecurityLabelSerializer().serialize(item.getLabel());
            itemElement->addNode(std::make_shared<XMLRawTextNode>(serializedLabel));
        }
        element.addNode(itemElement);
    }
    return element.serialize();
}

}
