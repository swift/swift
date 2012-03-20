/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/RosterItemExchangeSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

RosterItemExchangeSerializer::RosterItemExchangeSerializer() : GenericPayloadSerializer<RosterItemExchangePayload>() {
}

std::string RosterItemExchangeSerializer::serializePayload(boost::shared_ptr<RosterItemExchangePayload> roster)  const {
	XMLElement queryElement("x", "http://jabber.org/protocol/rosterx");
	foreach(const RosterItemExchangePayload::Item& item, roster->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
		itemElement->setAttribute("jid", item.getJID());
		itemElement->setAttribute("name", item.getName());

		switch (item.getAction()) {
			case RosterItemExchangePayload::Item::Add: itemElement->setAttribute("action", "add"); break;
			case RosterItemExchangePayload::Item::Modify: itemElement->setAttribute("action", "modify"); break;
			case RosterItemExchangePayload::Item::Delete: itemElement->setAttribute("action", "delete"); break;
		}

		foreach(const std::string& group, item.getGroups()) {
			boost::shared_ptr<XMLElement> groupElement(new XMLElement("group"));
			groupElement->addNode(boost::make_shared<XMLTextNode>(group));
			itemElement->addNode(groupElement);
		}

		queryElement.addNode(itemElement);
	}

	return queryElement.serialize();
}

}
