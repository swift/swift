/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Delay.h>

namespace Swift {
	class DelaySerializer : public GenericPayloadSerializer<Delay> {
		public:
			DelaySerializer();

			virtual std::string serializePayload(boost::shared_ptr<Delay>)  const;
	};
}


