/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCAdminPayloadParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {

void MUCAdminPayloadParser::handleTree(ParserElement::ref root) {
	foreach (ParserElement::ref itemElement, root->getChildren("item", "http://jabber.org/protocol/muc#admin")) {
		MUCItem item = MUCItemParser::itemFromTree(itemElement);
		getPayloadInternal()->addItem(item);
	}
}

}
