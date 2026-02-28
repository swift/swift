/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCDestroyPayloadParser.h>

namespace Swift {

void MUCDestroyPayloadParser::handleTree(ParserElement::ref root) {
    std::string ns = root->getNamespace();
    std::string jid = root->getAttributes().getAttribute("jid");
    if (!jid.empty()) {
        getPayloadInternal()->setNewVenue(JID(jid));
    }
    getPayloadInternal()->setReason(root->getChild("reason", ns)->getText());
}

}
