/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubOwnerSubscription.h>
#include <boost/shared_ptr.hpp>

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
