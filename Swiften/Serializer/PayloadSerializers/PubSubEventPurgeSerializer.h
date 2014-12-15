/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubEventPurge.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubEventPurgeSerializer : public GenericPayloadSerializer<PubSubEventPurge> {
		public:
			PubSubEventPurgeSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubEventPurgeSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubEventPurge>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
