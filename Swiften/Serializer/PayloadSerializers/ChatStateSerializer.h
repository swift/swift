#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/ChatState.h"

namespace Swift {
	class ChatStateSerializer : public GenericPayloadSerializer<ChatState> {
		public:
			ChatStateSerializer();

			virtual String serializePayload(boost::shared_ptr<ChatState> error)  const;
	};
}
