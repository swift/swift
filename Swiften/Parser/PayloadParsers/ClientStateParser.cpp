/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/ClientStateParser.h>

namespace Swift {

ClientStateParser::ClientStateParser() {
}

void ClientStateParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap&) {
    if (level_ == 0) {
        auto state = ClientState::ClientStateType::Active;
        if (element == "active") {
            state = ClientState::ClientStateType::Active;
        } else if (element == "inactive") {
            state = ClientState::ClientStateType::Inactive;
        }
        getPayloadInternal()->setClientState(state);
    }
    ++level_;
}

void ClientStateParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
}

void ClientStateParser::handleCharacterData(const std::string&) {

}

}
