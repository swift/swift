/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubConfigureSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubCreateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubDefaultSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemsSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubUnsubscribeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubPublishSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOptionsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubRetractSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOptionsSerializer.h>
#include <Swiften/Base/foreach.h>

using namespace Swift;

PubSubSerializer::PubSubSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
	pubsubSerializers.push_back(boost::make_shared<PubSubItemsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubCreateSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubPublishSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubOptionsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubAffiliationsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubRetractSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubDefaultSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubSubscriptionsSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubSubscribeSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubUnsubscribeSerializer>(serializers));
	pubsubSerializers.push_back(boost::make_shared<PubSubSubscriptionSerializer>(serializers));
}

PubSubSerializer::~PubSubSerializer() {
}

std::string PubSubSerializer::serializePayload(boost::shared_ptr<PubSub> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("pubsub", "http://jabber.org/protocol/pubsub");
	boost::shared_ptr<PubSubPayload> p = payload->getPayload();
	foreach(boost::shared_ptr<PayloadSerializer> serializer, pubsubSerializers) {
		if (serializer->canSerialize(p)) {
			element.addNode(boost::make_shared<XMLRawTextNode>(serializer->serialize(p)));
			if (boost::shared_ptr<PubSubCreate> create = boost::dynamic_pointer_cast<PubSubCreate>(p)) {
				element.addNode(boost::make_shared<XMLRawTextNode>(boost::make_shared<PubSubConfigureSerializer>(serializers)->serialize(create->getConfigure())));
			}
			if (boost::shared_ptr<PubSubSubscribe> subscribe = boost::dynamic_pointer_cast<PubSubSubscribe>(p)) {
				element.addNode(boost::make_shared<XMLRawTextNode>(boost::make_shared<PubSubConfigureSerializer>(serializers)->serialize(subscribe->getOptions())));
			}
		}
	}
	return element.serialize();
}


