/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleIBBTransportPayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

JingleIBBTransportPayloadSerializer::JingleIBBTransportPayloadSerializer() {
}

std::string JingleIBBTransportPayloadSerializer::serializePayload(boost::shared_ptr<JingleIBBTransportPayload> payload) const {
	XMLElement payloadXML("transport", "urn:xmpp:jingle:transports:ibb:1");
	payloadXML.setAttribute("block-size", boost::lexical_cast<std::string>(payload->getBlockSize()));
	payloadXML.setAttribute("sid", payload->getSessionID());

	return payloadXML.serialize();
}

}
