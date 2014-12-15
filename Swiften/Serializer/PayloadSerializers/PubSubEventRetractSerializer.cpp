/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventRetractSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubEventRetractSerializer::PubSubEventRetractSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventRetractSerializer::~PubSubEventRetractSerializer() {
}

std::string PubSubEventRetractSerializer::serializePayload(boost::shared_ptr<PubSubEventRetract> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("retract", "http://jabber.org/protocol/pubsub#event");
	element.setAttribute("id", payload->getID());
	return element.serialize();
}


