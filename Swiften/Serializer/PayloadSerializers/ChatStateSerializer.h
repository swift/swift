/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/ChatState.h>

namespace Swift {
	class ChatStateSerializer : public GenericPayloadSerializer<ChatState> {
		public:
			ChatStateSerializer();

			virtual std::string serializePayload(boost::shared_ptr<ChatState> error)  const;
	};
}
