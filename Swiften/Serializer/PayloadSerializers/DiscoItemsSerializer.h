/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/DiscoItems.h>

namespace Swift {
	class DiscoItemsSerializer : public GenericPayloadSerializer<DiscoItems> {
		public:
			DiscoItemsSerializer();

			virtual std::string serializePayload(boost::shared_ptr<DiscoItems>)  const;
	};
}

