/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXInvitationParser.h>

namespace Swift {

void MIXInvitationParser::handleTree(ParserElement::ref root) {
    for (const auto& child : root->getAllChildren()) {
        if (child->getName() == "inviter" && child->getNamespace() == root->getNamespace()) {
            if (boost::optional<JID> jid = JID::parse(child->getText())) {
                getPayloadInternal()->setInviter(*jid);
            }
        } else if (child->getName() == "invitee" && child->getNamespace() == root->getNamespace()) {
            if (boost::optional<JID> jid = JID::parse(child->getText())) {
                getPayloadInternal()->setInvitee(*jid);
            }
        } else if (child->getName() == "channel" && child->getNamespace() == root->getNamespace()) {
            if (boost::optional<JID> jid = JID::parse(child->getText())) {
                getPayloadInternal()->setChannel(*jid);
            }
        } else if (child->getName() == "token" && child->getNamespace() == root->getNamespace()) {
            getPayloadInternal()->setToken(child->getText());
        }
    }
}
}
