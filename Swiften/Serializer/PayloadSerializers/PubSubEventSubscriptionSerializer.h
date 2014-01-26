/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
