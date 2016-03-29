/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerAffiliations.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerAffiliationsSerializer : public GenericPayloadSerializer<PubSubOwnerAffiliations> {
		public:
			PubSubOwnerAffiliationsSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerAffiliationsSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerAffiliations>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
