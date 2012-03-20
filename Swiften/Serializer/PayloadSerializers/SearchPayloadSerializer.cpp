/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/SearchPayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

namespace Swift {

SearchPayloadSerializer::SearchPayloadSerializer() {
}

std::string SearchPayloadSerializer::serializePayload(boost::shared_ptr<SearchPayload> searchPayload)	const {
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

	foreach(const SearchPayload::Item& item, searchPayload->getItems()) {
		XMLElement::ref itemElement(new XMLElement("item"));
		itemElement->setAttribute("jid", item.jid);
		itemElement->addNode(XMLElement::ref(new XMLElement("first", "", item.first)));
		itemElement->addNode(XMLElement::ref(new XMLElement("last", "", item.last)));
		itemElement->addNode(XMLElement::ref(new XMLElement("nick", "", item.nick)));
		itemElement->addNode(XMLElement::ref(new XMLElement("email", "", item.email)));

		searchElement.addNode(itemElement);
	}

	if (Form::ref form = searchPayload->getForm()) {
		searchElement.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
	}

	return searchElement.serialize();
}

}
