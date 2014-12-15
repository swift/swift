/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/StreamInitiation.h>

namespace Swift {
	class SWIFTEN_API StreamInitiationSerializer : public GenericPayloadSerializer<StreamInitiation> {
		public:
			StreamInitiationSerializer();

			virtual std::string serializePayload(boost::shared_ptr<StreamInitiation>)  const;
	};
}
