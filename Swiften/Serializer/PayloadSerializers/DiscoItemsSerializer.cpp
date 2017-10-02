/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/DiscoItemsSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

DiscoItemsSerializer::DiscoItemsSerializer() : GenericPayloadSerializer<DiscoItems>() {
}

std::string DiscoItemsSerializer::serializePayload(std::shared_ptr<DiscoItems> discoItems)  const {
    XMLElement queryElement("query", "http://jabber.org/protocol/disco#items");
    if (!discoItems->getNode().empty()) {
        queryElement.setAttribute("node", discoItems->getNode());
    }
    for (const auto& item : discoItems->getItems()) {
        std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
        itemElement->setAttribute("name", item.getName());
        itemElement->setAttribute("jid", item.getJID());
        if (!item.getNode().empty()) {
            itemElement->setAttribute("node", item.getNode());
        }
        queryElement.addNode(itemElement);
    }
    return queryElement.serialize();
}

}
