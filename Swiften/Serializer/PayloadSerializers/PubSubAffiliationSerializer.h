/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubAffiliation.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubAffiliationSerializer : public GenericPayloadSerializer<PubSubAffiliation> {
		public:
			PubSubAffiliationSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubAffiliationSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubAffiliation>) const SWIFTEN_OVERRIDE;

		private:
			static std::string serializeType(PubSubAffiliation::Type);

		private:
			PayloadSerializerCollection* serializers;
	};
}
