/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerDelete.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerDeleteSerializer : public GenericPayloadSerializer<PubSubOwnerDelete> {
		public:
			PubSubOwnerDeleteSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerDeleteSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerDelete>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
