/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCDestroyPayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MUCDestroyPayloadSerializer::MUCDestroyPayloadSerializer() : GenericPayloadSerializer<MUCDestroyPayload>() {
}

std::string MUCDestroyPayloadSerializer::serializePayload(boost::shared_ptr<MUCDestroyPayload> payload)  const {
	XMLElement mucElement("destroy", "");
	if (!payload->getReason().empty()) {
		XMLElement::ref reason = boost::make_shared<XMLElement>("reason", "");
		reason->addNode(boost::make_shared<XMLTextNode>(payload->getReason()));
		mucElement.addNode(reason);
	}
	if (payload->getNewVenue().isValid()) {
		mucElement.setAttribute("jid", payload->getNewVenue().toString());
	}
	return mucElement.serialize();
}




}
