/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubSubscription.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubSubscriptionSerializer : public GenericPayloadSerializer<PubSubSubscription> {
		public:
			PubSubSubscriptionSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubSubscriptionSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubSubscription>) const SWIFTEN_OVERRIDE;

		private:
			static std::string serializeSubscriptionType(PubSubSubscription::SubscriptionType);

		private:
			PayloadSerializerCollection* serializers;
	};
}
