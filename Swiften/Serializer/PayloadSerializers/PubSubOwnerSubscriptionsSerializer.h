/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubOwnerSubscriptions.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerSubscriptionsSerializer : public GenericPayloadSerializer<PubSubOwnerSubscriptions> {
		public:
			PubSubOwnerSubscriptionsSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerSubscriptionsSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerSubscriptions>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
