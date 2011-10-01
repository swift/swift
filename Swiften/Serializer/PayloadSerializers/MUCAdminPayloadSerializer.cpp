/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>

#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/MUCItemSerializer.h>


namespace Swift {

MUCAdminPayloadSerializer::MUCAdminPayloadSerializer() : GenericPayloadSerializer<MUCAdminPayload>() {
}

std::string MUCAdminPayloadSerializer::serializePayload(boost::shared_ptr<MUCAdminPayload> payload)  const {
	XMLElement mucElement("query", "http://jabber.org/protocol/muc#admin");
	foreach (const MUCItem& item, payload->getItems()) {
		mucElement.addNode(MUCItemSerializer::itemToElement(item));
	}
	return mucElement.serialize();
}


}
