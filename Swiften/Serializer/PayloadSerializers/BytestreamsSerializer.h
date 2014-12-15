/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Bytestreams.h>

namespace Swift {
	class PayloadSerializerCollection;

	class BytestreamsSerializer : public GenericPayloadSerializer<Bytestreams> {
		public:
			BytestreamsSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Bytestreams>)  const;
	};
}
