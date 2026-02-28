/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCAdminPayloadParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {

void MUCAdminPayloadParser::handleTree(ParserElement::ref root) {
    for (const auto& itemElement : root->getChildren("item", "http://jabber.org/protocol/muc#admin")) {
        MUCItem item = MUCItemParser::itemFromTree(itemElement);
        getPayloadInternal()->addItem(item);
    }
}

}
