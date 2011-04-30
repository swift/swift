/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/SearchPayloadParser.h>

#include <boost/cast.hpp>

#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

SearchPayloadParser::SearchPayloadParser() : level(TopLevel), formParser(NULL)  {
	formParserFactory = new FormParserFactory();
}

SearchPayloadParser::~SearchPayloadParser() {
	delete formParserFactory;
}

void SearchPayloadParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	if (level == TopLevel) {
	}
	else if (level == PayloadLevel) {
		if (element == "x" && ns == "jabber:x:data") {
			assert(!formParser);
			formParser = boost::polymorphic_downcast<FormParser*>(formParserFactory->createPayloadParser());
		}
		else if (element == "item") {
			assert(!currentItem);
			currentItem.reset(SearchPayload::Item());
			currentItem->jid = JID(attributes.getAttribute("jid"));
		}
		else {
			currentText.clear();
		}
	}
	else if (level == ItemLevel && currentItem) {
		currentText.clear();
	}

	if (formParser) {
		formParser->handleStartElement(element, ns, attributes);
	}

	++level;
}

void SearchPayloadParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;

	if (formParser) {
		formParser->handleEndElement(element, ns);
	}

	if (level == TopLevel) {
	}
	else if (level == PayloadLevel) {
		if (formParser) {
			getPayloadInternal()->setForm(formParser->getPayloadInternal());
			delete formParser;
			formParser = NULL;
		}
		else if (element == "item") {
			assert(currentItem);
			getPayloadInternal()->addItem(*currentItem);
			currentItem.reset();
		}
		else if (element == "instructions") {
			getPayloadInternal()->setInstructions(currentText);
		}
		else if (element == "nick") {
			getPayloadInternal()->setNick(currentText);
		}
		else if (element == "first") {
			getPayloadInternal()->setFirst(currentText);
		}
		else if (element == "last") {
			getPayloadInternal()->setLast(currentText);
		}
		else if (element == "email") {
			getPayloadInternal()->setEMail(currentText);
		}
	}
	else if (level == ItemLevel && currentItem) {
		if (element == "nick") {
			currentItem->nick = currentText;
		}
		else if (element == "first") {
			currentItem->first = currentText;
		}
		else if (element == "last") {
			currentItem->last = currentText;
		}
		else if (element == "email") {
			currentItem->email = currentText;
		}
	}
}

void SearchPayloadParser::handleCharacterData(const std::string& data) {
	if (formParser) {
		formParser->handleCharacterData(data);
	}
	else {
		currentText += data;
	}
}

}
