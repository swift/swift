/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/RosterItemExchangeParser.h"
#include "Swiften/Parser/SerializingParser.h"

namespace Swift {

RosterItemExchangeParser::RosterItemExchangeParser() : level_(TopLevel), inItem_(false) {
}

void RosterItemExchangeParser::handleStartElement(const std::string& element, const std::string& /*ns*/, const AttributeMap& attributes) {
	if (level_ == PayloadLevel) {
		if (element == "item") {
			inItem_ = true;

			currentItem_ = RosterItemExchangePayload::Item();

			currentItem_.jid = JID(attributes.getAttribute("jid"));
			currentItem_.name = attributes.getAttribute("name");

			std::string action = attributes.getAttribute("action");
			if (action == "add") {
				currentItem_.action = RosterItemExchangePayload::Add;
			}
			else if (action == "modify") {
				currentItem_.action = RosterItemExchangePayload::Modify;
			}
			else if (action == "delete") {
				currentItem_.action = RosterItemExchangePayload::Delete;
			}
			else {
				// Add is default action according to XEP
				currentItem_.action = RosterItemExchangePayload::Add;
			}
		}
	}
	else if (level_ == ItemLevel) {
		if (element == "group") {
			currentText_ = "";
		}
	}
	++level_;
}

void RosterItemExchangeParser::handleEndElement(const std::string& element, const std::string& /*ns*/) {
	--level_;
	if (level_ == PayloadLevel) {
		if (inItem_) {
			getPayloadInternal()->addItem(currentItem_);
			inItem_ = false;
		}
	}
	else if (level_ == ItemLevel) {
		if (element == "group") {
			currentItem_.groups.push_back(currentText_);
		}
	}
}

void RosterItemExchangeParser::handleCharacterData(const std::string& data) {
	currentText_ += data;
}

}
