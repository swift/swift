/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/DiscoItemsSerializer.h>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

DiscoItemsSerializer::DiscoItemsSerializer() : GenericPayloadSerializer<DiscoItems>() {
}

std::string DiscoItemsSerializer::serializePayload(boost::shared_ptr<DiscoItems> discoItems)  const {
	XMLElement queryElement("query", "http://jabber.org/protocol/disco#items");
	if (!discoItems->getNode().empty()) {
		queryElement.setAttribute("node", discoItems->getNode());
	}
	foreach(const DiscoItems::Item& item, discoItems->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
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
