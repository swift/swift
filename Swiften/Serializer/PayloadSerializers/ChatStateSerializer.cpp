#include "Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h"

namespace Swift {

ChatStateSerializer::ChatStateSerializer() : GenericPayloadSerializer<ChatState>() {
}

String ChatStateSerializer::serializePayload(boost::shared_ptr<ChatState> chatState)  const {
	String result("<");
	switch (chatState->getChatState()) {
		case ChatState::Active: result += "active"; break;
		case ChatState::Composing: result += "composing"; break;
		case ChatState::Paused: result += "paused"; break;
		case ChatState::Inactive: result += "inactive"; break;
		case ChatState::Gone: result += "gone"; break;
		default: result += "gone"; break;
	}
	result += " xmlns=\"http://jabber.org/protocol/chatstates\"/>";
	return result;
}

}
