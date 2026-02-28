/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXParticipantParser.h>

namespace Swift {

void MIXParticipantParser::handleTree(ParserElement::ref root) {
    for (const auto& child : root->getAllChildren()) {
        if (child->getName() == "nick" && child->getNamespace() == root->getNamespace()) {
            getPayloadInternal()->setNick(child->getText());
        }
        else if (child->getName() == "jid" && child->getNamespace() == root->getNamespace()) {
            if (boost::optional<JID> jid = JID::parse(child->getText())) {
                getPayloadInternal()->setJID(*jid);
            }
        }
    }
}

}
