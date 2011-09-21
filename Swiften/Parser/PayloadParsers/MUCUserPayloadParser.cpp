/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {

void MUCUserPayloadParser::handleTree(ParserElement::ref root) {
	foreach (ParserElement::ref itemElement, root->getChildren("item", "http://jabber.org/protocol/muc#user")) {
		MUCItem item = MUCItemParser::itemFromTree(itemElement);
		getPayloadInternal()->addItem(item);
	}
	foreach (ParserElement::ref statusElement, root->getChildren("item", "http://jabber.org/protocol/muc#user")) {
		MUCUserPayload::StatusCode status;
		try {
			status.code = boost::lexical_cast<int>(statusElement->getAttributes().getAttribute("code").c_str());
			getPayloadInternal()->addStatusCode(status);
		} catch (boost::bad_lexical_cast&) {
		}
	}
}

}
