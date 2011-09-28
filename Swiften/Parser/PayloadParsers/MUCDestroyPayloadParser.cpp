/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCDestroyPayloadParser.h>

#include <Swiften/Base/foreach.h>

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
