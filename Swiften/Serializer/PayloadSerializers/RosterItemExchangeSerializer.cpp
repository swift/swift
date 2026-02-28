/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/RosterItemExchangeSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

RosterItemExchangeSerializer::RosterItemExchangeSerializer() : GenericPayloadSerializer<RosterItemExchangePayload>() {
}

std::string RosterItemExchangeSerializer::serializePayload(std::shared_ptr<RosterItemExchangePayload> roster)  const {
    XMLElement queryElement("x", "http://jabber.org/protocol/rosterx");
    for (const auto& item : roster->getItems()) {
        std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
        itemElement->setAttribute("jid", item.getJID());
        itemElement->setAttribute("name", item.getName());

        switch (item.getAction()) {
            case RosterItemExchangePayload::Item::Add: itemElement->setAttribute("action", "add"); break;
            case RosterItemExchangePayload::Item::Modify: itemElement->setAttribute("action", "modify"); break;
            case RosterItemExchangePayload::Item::Delete: itemElement->setAttribute("action", "delete"); break;
        }

        for (const auto& group : item.getGroups()) {
            std::shared_ptr<XMLElement> groupElement(new XMLElement("group"));
            groupElement->addNode(std::make_shared<XMLTextNode>(group));
            itemElement->addNode(groupElement);
        }

        queryElement.addNode(itemElement);
    }

    return queryElement.serialize();
}

}
