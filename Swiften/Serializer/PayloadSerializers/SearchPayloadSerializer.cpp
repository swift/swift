/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

SearchPayloadSerializer::SearchPayloadSerializer() {
}

std::string SearchPayloadSerializer::serializePayload(std::shared_ptr<SearchPayload> searchPayload)    const {
    XMLElement searchElement("query", "jabber:iq:search");

    if (searchPayload->getInstructions()) {
        searchElement.addNode(XMLElement::ref(new XMLElement("instructions", "", *searchPayload->getInstructions())));
    }

    if (searchPayload->getNick()) {
        searchElement.addNode(XMLElement::ref(new XMLElement("nick", "", *searchPayload->getNick())));
    }

    if (searchPayload->getFirst()) {
        searchElement.addNode(XMLElement::ref(new XMLElement("first", "", *searchPayload->getFirst())));
    }

    if (searchPayload->getLast()) {
        searchElement.addNode(XMLElement::ref(new XMLElement("last", "", *searchPayload->getLast())));
    }

    if (searchPayload->getEMail()) {
        searchElement.addNode(XMLElement::ref(new XMLElement("email", "", *searchPayload->getEMail())));
    }

    for (const auto& item : searchPayload->getItems()) {
        XMLElement::ref itemElement(new XMLElement("item"));
        itemElement->setAttribute("jid", item.jid);
        itemElement->addNode(XMLElement::ref(new XMLElement("first", "", item.first)));
        itemElement->addNode(XMLElement::ref(new XMLElement("last", "", item.last)));
        itemElement->addNode(XMLElement::ref(new XMLElement("nick", "", item.nick)));
        itemElement->addNode(XMLElement::ref(new XMLElement("email", "", item.email)));

        searchElement.addNode(itemElement);
    }

    if (Form::ref form = searchPayload->getForm()) {
        searchElement.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
    }

    return searchElement.serialize();
}

}
