/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCOwnerPayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	class SWIFTEN_API MUCOwnerPayloadSerializer : public GenericPayloadSerializer<MUCOwnerPayload> {
		public:
			MUCOwnerPayloadSerializer(PayloadSerializerCollection* serializers);
			virtual std::string serializePayload(boost::shared_ptr<MUCOwnerPayload> version)  const;
		private:
			PayloadSerializerCollection* serializers;
	};
}

