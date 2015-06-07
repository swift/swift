/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/IBB.h>

namespace Swift {
	class SWIFTEN_API IBBSerializer : public GenericPayloadSerializer<IBB> {
		public:
			IBBSerializer();

			virtual std::string serializePayload(boost::shared_ptr<IBB>)  const;
	};
}
