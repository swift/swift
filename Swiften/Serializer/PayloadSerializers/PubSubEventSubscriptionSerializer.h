/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubEventSubscription.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubEventSubscriptionSerializer : public GenericPayloadSerializer<PubSubEventSubscription> {
		public:
			PubSubEventSubscriptionSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubEventSubscriptionSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubEventSubscription>) const SWIFTEN_OVERRIDE;

		private:
			static std::string serializeSubscriptionType(PubSubEventSubscription::SubscriptionType);

		private:
			PayloadSerializerCollection* serializers;
	};
}
