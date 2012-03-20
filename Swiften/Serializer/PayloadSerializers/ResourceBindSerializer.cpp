/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

ResourceBindSerializer::ResourceBindSerializer() : GenericPayloadSerializer<ResourceBind>() {
}

std::string ResourceBindSerializer::serializePayload(boost::shared_ptr<ResourceBind> resourceBind)  const {
	XMLElement bindElement("bind", "urn:ietf:params:xml:ns:xmpp-bind");
	if (resourceBind->getJID().isValid()) {
		boost::shared_ptr<XMLElement> jidNode(new XMLElement("jid"));
		jidNode->addNode(boost::make_shared<XMLTextNode>(resourceBind->getJID().toString()));
		bindElement.addNode(jidNode);
	}
	else if (!resourceBind->getResource().empty()) {
		boost::shared_ptr<XMLElement> resourceNode(new XMLElement("resource"));
		resourceNode->addNode(boost::make_shared<XMLTextNode>(resourceBind->getResource()));
		bindElement.addNode(resourceNode);
	}
	return bindElement.serialize();
}

}
