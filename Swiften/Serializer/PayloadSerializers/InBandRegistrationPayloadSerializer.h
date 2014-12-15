/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/InBandRegistrationPayload.h>

namespace Swift {
	class SWIFTEN_API InBandRegistrationPayloadSerializer : public GenericPayloadSerializer<InBandRegistrationPayload> {
		public:
			InBandRegistrationPayloadSerializer();

			virtual std::string serializePayload(boost::shared_ptr<InBandRegistrationPayload>)  const;
	};
}
