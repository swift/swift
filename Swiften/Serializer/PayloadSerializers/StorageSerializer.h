/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Storage.h>

namespace Swift {
	class StorageSerializer : public GenericPayloadSerializer<Storage> {
		public:
			StorageSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Storage>)  const;
	};
}
