/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCUserPayloadSerializer.h>

#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/MUCItemSerializer.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {

MUCUserPayloadSerializer::MUCUserPayloadSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<MUCUserPayload>(), serializers(serializers) {
}

std::string MUCUserPayloadSerializer::serializePayload(boost::shared_ptr<MUCUserPayload> payload)  const {
	XMLElement mucElement("x", "http://jabber.org/protocol/muc#user");
	foreach (const MUCUserPayload::StatusCode statusCode, payload->getStatusCodes()) {
		boost::shared_ptr<XMLElement> statusElement(new XMLElement("status"));
		std::ostringstream code;
		code << statusCode.code;
		statusElement->setAttribute("code", code.str());
		mucElement.addNode(statusElement);
	}
	foreach (const MUCItem& item, payload->getItems()) {
		mucElement.addNode(MUCItemSerializer::itemToElement(item));
	}

	if (payload->getPassword()) {
		boost::shared_ptr<XMLElement> passwordElement = boost::make_shared<XMLElement>("password");
		passwordElement->addNode(boost::make_shared<XMLTextNode>(*payload->getPassword()));
	}

	if (payload->getInvite()) {
		MUCUserPayload::Invite invite = *payload->getInvite();
		boost::shared_ptr<XMLElement> inviteElement = boost::make_shared<XMLElement>("invite");
		if (invite.to.isValid()) {
			inviteElement->setAttribute("to", invite.to.toString());
		}
		if (invite.from.isValid()) {
			inviteElement->setAttribute("from", invite.from.toString());
		}
		if (!invite.reason.empty()) {
			boost::shared_ptr<XMLElement> reasonElement = boost::make_shared<XMLElement>("reason");
			reasonElement->addNode(boost::make_shared<XMLTextNode>(invite.reason));
		}
		mucElement.addNode(inviteElement);
	}

	boost::shared_ptr<Payload> childPayload = payload->getPayload();
	if (childPayload) {
		PayloadSerializer* serializer = serializers->getPayloadSerializer(childPayload);
		if (serializer) {
			mucElement.addNode(boost::make_shared<XMLRawTextNode>(serializer->serialize(childPayload)));
		}
	}
	return mucElement.serialize();
}




}
