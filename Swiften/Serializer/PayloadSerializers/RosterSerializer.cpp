/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

RosterSerializer::RosterSerializer() : GenericPayloadSerializer<RosterPayload>() {
}

std::string RosterSerializer::serializePayload(std::shared_ptr<RosterPayload> roster)  const {
    XMLElement queryElement("query", "jabber:iq:roster");
    if (roster->getVersion()) {
        queryElement.setAttribute("ver", *roster->getVersion());
    }
    for (const auto& item : roster->getItems()) {
        std::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
        itemElement->setAttribute("jid", item.getJID());
        itemElement->setAttribute("name", item.getName());

        switch (item.getSubscription()) {
            case RosterItemPayload::To: itemElement->setAttribute("subscription", "to"); break;
            case RosterItemPayload::From: itemElement->setAttribute("subscription", "from"); break;
            case RosterItemPayload::Both: itemElement->setAttribute("subscription", "both"); break;
            case RosterItemPayload::Remove: itemElement->setAttribute("subscription", "remove"); break;
            case RosterItemPayload::None: itemElement->setAttribute("subscription", "none"); break;
        }

        if (item.getSubscriptionRequested()) {
            itemElement->setAttribute("ask", "subscribe");
        }

        for (const auto& group : item.getGroups()) {
            std::shared_ptr<XMLElement> groupElement(new XMLElement("group"));
            groupElement->addNode(std::make_shared<XMLTextNode>(group));
            itemElement->addNode(groupElement);
        }

        if (!item.getUnknownContent().empty()) {
            itemElement->addNode(std::make_shared<XMLRawTextNode>(item.getUnknownContent()));
        }


        queryElement.addNode(itemElement);
    }

    return queryElement.serialize();
}

}
