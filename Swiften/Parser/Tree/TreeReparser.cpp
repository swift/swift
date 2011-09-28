/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/Tree/TreeReparser.h>

#include <boost/lexical_cast.hpp>
#include <utility>
#include <deque>

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParser.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {

typedef std::pair<ParserElement::ref, bool> ElementState;

boost::shared_ptr<Payload> TreeReparser::parseTree(ParserElement::ref root, PayloadParserFactoryCollection* collection) {
	PayloadParser* parser = collection->getPayloadParserFactory(root->getName(), root->getNamespace(), root->getAttributes())->createPayloadParser();
	std::deque<ElementState > stack;
	stack.push_back(ElementState(root, true));
	while (!stack.empty()) {
		ElementState current = stack.back();
		stack.pop_back();
		if (current.second) {
			stack.push_back(ElementState(current.first, false));
			parser->handleStartElement(current.first->getName(), current.first->getNamespace(), current.first->getAttributes());
			foreach(ParserElement::ref child, current.first->getAllChildren()) {
				stack.push_back(ElementState(child, true));
			}
		} else {
			parser->handleCharacterData(current.first->getText());
			parser->handleEndElement(current.first->getName(), current.first->getNamespace());
		}

	}

	boost::shared_ptr<Payload> payload = parser->getPayload();
	delete parser;
	return payload;
}

}
