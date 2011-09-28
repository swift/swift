/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Parser/Tree/TreeReparser.h>

namespace Swift {

void MUCUserPayloadParser::handleTree(ParserElement::ref root) {
	foreach (ParserElement::ref child, root->getAllChildren()) {
		if (child->getName() == "item" && child->getNamespace() == root->getNamespace()) {
			MUCItem item = MUCItemParser::itemFromTree(child);
			getPayloadInternal()->addItem(item);
		}
		else if (child->getName() == "status" && child->getNamespace() == root->getNamespace()) {
			MUCUserPayload::StatusCode status;
			try {
				status.code = boost::lexical_cast<int>(child->getAttributes().getAttribute("code").c_str());
				getPayloadInternal()->addStatusCode(status);
			} catch (boost::bad_lexical_cast&) {
			}
		}
		else {
			getPayloadInternal()->setPayload(TreeReparser::parseTree(child, factories));
		}
	}
}

}
