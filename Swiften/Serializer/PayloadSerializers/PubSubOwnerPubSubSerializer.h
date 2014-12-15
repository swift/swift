/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubOwnerPubSub.h>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerPubSubSerializer : public GenericPayloadSerializer<PubSubOwnerPubSub> {
		public:
			PubSubOwnerPubSubSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerPubSubSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerPubSub>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
			std::vector< boost::shared_ptr<PayloadSerializer> > pubsubSerializers;
	};
}
