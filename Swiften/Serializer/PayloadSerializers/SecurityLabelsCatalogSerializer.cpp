/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.h>

namespace Swift {

SecurityLabelsCatalogSerializer::SecurityLabelsCatalogSerializer() : GenericPayloadSerializer<SecurityLabelsCatalog>() {
}

std::string SecurityLabelsCatalogSerializer::serializePayload(boost::shared_ptr<SecurityLabelsCatalog> catalog)  const {
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
	foreach (const SecurityLabelsCatalog::Item& item, catalog->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
		itemElement->setAttribute("selector", item.getSelector());
		if (item.getIsDefault()) {
			itemElement->setAttribute("default", "true");
		}
		if (item.getLabel()) {
			std::string serializedLabel = SecurityLabelSerializer().serialize(item.getLabel());
			itemElement->addNode(boost::make_shared<XMLRawTextNode>(serializedLabel));
		}
		element.addNode(itemElement);
	}
	return element.serialize();
}

}
