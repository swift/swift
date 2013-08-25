/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPubSubSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDeleteSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionsSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDefaultSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPurgeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerConfigureSerializer.h>
#include <Swiften/Base/foreach.h>

using namespace Swift;

PubSubOwnerPubSubSerializer::PubSubOwnerPubSubSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerConfigureSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerSubscriptionsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerDefaultSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerPurgeSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerAffiliationsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOwnerDeleteSerializer>(serializers));
}

PubSubOwnerPubSubSerializer::~PubSubOwnerPubSubSerializer() {
}

std::string PubSubOwnerPubSubSerializer::serializePayload(boost::shared_ptr<PubSubOwnerPubSub> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("pubsub", "http://jabber.org/protocol/pubsub#owner");
	boost::shared_ptr<PubSubOwnerPayload> p = payload->getPayload();
	foreach(boost::shared_ptr<PayloadSerializer> serializer, pubsubSerializers) {
		if (serializer->canSerialize(p)) {
			element.addNode(boost::make_shared<XMLRawTextNode>(serializer->serialize(p)));
		}
	}
	return element.serialize();
}


