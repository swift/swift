/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ClientStateSerializer.h>

namespace Swift {

ClientStateSerializer::ClientStateSerializer() : GenericPayloadSerializer<ClientState>() {
}

std::string ClientStateSerializer::serializePayload(std::shared_ptr<ClientState> clientState)  const {
    std::string result("<");
    switch (clientState->getClientState()) {
        case ClientState::ClientStateType::Active: result += "active"; break;
        case ClientState::ClientStateType::Inactive: result += "inactive"; break;
    }
    result += " xmlns=\"urn:xmpp:csi:0\"/>";
    return result;
}

}
