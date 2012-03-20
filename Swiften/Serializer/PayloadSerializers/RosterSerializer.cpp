/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/RosterSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

RosterSerializer::RosterSerializer() : GenericPayloadSerializer<RosterPayload>() {
}

std::string RosterSerializer::serializePayload(boost::shared_ptr<RosterPayload> roster)  const {
	XMLElement queryElement("query", "jabber:iq:roster");
	if (roster->getVersion()) {
		queryElement.setAttribute("ver", *roster->getVersion());
	}
	foreach(const RosterItemPayload& item, roster->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
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

		foreach(const std::string& group, item.getGroups()) {
			boost::shared_ptr<XMLElement> groupElement(new XMLElement("group"));
			groupElement->addNode(boost::make_shared<XMLTextNode>(group));
			itemElement->addNode(groupElement);
		}

		if (!item.getUnknownContent().empty()) {
			itemElement->addNode(boost::make_shared<XMLRawTextNode>(item.getUnknownContent()));
		}


		queryElement.addNode(itemElement);
	}

	return queryElement.serialize();
}

}
