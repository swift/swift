/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCDestroyPayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	class MUCDestroyPayloadSerializer : public GenericPayloadSerializer<MUCDestroyPayload> {
		public:
			MUCDestroyPayloadSerializer();
			virtual std::string serializePayload(boost::shared_ptr<MUCDestroyPayload> version)  const;
	};
}

