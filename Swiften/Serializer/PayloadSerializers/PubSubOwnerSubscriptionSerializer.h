/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerSubscription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerSubscriptionSerializer : public GenericPayloadSerializer<PubSubOwnerSubscription> {
		public:
			PubSubOwnerSubscriptionSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerSubscriptionSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerSubscription>) const SWIFTEN_OVERRIDE;

		private:
			static std::string serializeSubscriptionType(PubSubOwnerSubscription::SubscriptionType);

		private:
			PayloadSerializerCollection* serializers;
	};
}
