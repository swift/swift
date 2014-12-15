/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCPayload.h>

namespace Swift {
	class MUCPayloadSerializer : public GenericPayloadSerializer<MUCPayload> {
		public:
			MUCPayloadSerializer();
			virtual std::string serializePayload(boost::shared_ptr<MUCPayload> version)  const;
	};
}

