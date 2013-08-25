/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubOwnerSubscriptionSerializer::PubSubOwnerSubscriptionSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerSubscriptionSerializer::~PubSubOwnerSubscriptionSerializer() {
}

std::string PubSubOwnerSubscriptionSerializer::serializePayload(boost::shared_ptr<PubSubOwnerSubscription> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("subscription", "http://jabber.org/protocol/pubsub#owner");
	element.setAttribute("jid", payload->getJID());
	element.setAttribute("subscription", serializeSubscriptionType(payload->getSubscription()));
	return element.serialize();
}

std::string PubSubOwnerSubscriptionSerializer::serializeSubscriptionType(PubSubOwnerSubscription::SubscriptionType value) {
	switch (value) {
		case PubSubOwnerSubscription::None: return "none";
		case PubSubOwnerSubscription::Pending: return "pending";
		case PubSubOwnerSubscription::Subscribed: return "subscribed";
		case PubSubOwnerSubscription::Unconfigured: return "unconfigured";
	}
	assert(false);
	return "";
}
