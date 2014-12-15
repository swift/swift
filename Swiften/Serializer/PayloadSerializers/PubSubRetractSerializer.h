/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubRetract.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubRetractSerializer : public GenericPayloadSerializer<PubSubRetract> {
		public:
			PubSubRetractSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubRetractSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubRetract>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
