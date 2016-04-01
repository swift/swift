/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h>

namespace Swift {

ChatStateSerializer::ChatStateSerializer() : GenericPayloadSerializer<ChatState>() {
}

std::string ChatStateSerializer::serializePayload(std::shared_ptr<ChatState> chatState)  const {
    std::string result("<");
    switch (chatState->getChatState()) {
        case ChatState::Active: result += "active"; break;
        case ChatState::Composing: result += "composing"; break;
        case ChatState::Paused: result += "paused"; break;
        case ChatState::Inactive: result += "inactive"; break;
        case ChatState::Gone: result += "gone"; break;
    }
    result += " xmlns=\"http://jabber.org/protocol/chatstates\"/>";
    return result;
}

}
