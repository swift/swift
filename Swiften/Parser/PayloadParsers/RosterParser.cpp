#include "Swiften/Parser/PayloadParsers/RosterParser.h"

namespace Swift {

RosterParser::RosterParser() : level_(TopLevel) {
}

void RosterParser::handleStartElement(const String& element, const String&, const AttributeMap& attributes) {
	if (level_ == PayloadLevel) {
		if (element == "item") {
			inItem_ = true;
			currentItem_ = RosterItemPayload();

			currentItem_.setJID(JID(attributes.getAttribute("jid")));
			currentItem_.setName(attributes.getAttribute("name"));

			String subscription = attributes.getAttribute("subscription");
			if (subscription == "both") {
				currentItem_.setSubscription(RosterItemPayload::Both);
			}
			else if (subscription == "to") {
				currentItem_.setSubscription(RosterItemPayload::To);
			}
			else if (subscription == "from") {
				currentItem_.setSubscription(RosterItemPayload::From);
			}
			else if (subscription == "remove") {
				currentItem_.setSubscription(RosterItemPayload::Remove);
			}
			else {
				currentItem_.setSubscription(RosterItemPayload::None);
			}

			if (attributes.getAttribute("ask") == "subscribe") {
				currentItem_.setSubscriptionRequested();
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

void RosterParser::handleEndElement(const String& element, const String&) {
	--level_;
	if (level_ == PayloadLevel) {
		if (inItem_) {
			getPayloadInternal()->addItem(currentItem_);
			inItem_ = false;
		}
	}
	else if (level_ == ItemLevel) {
		if (element == "group") {
			currentItem_.addGroup(currentText_);
		}
	}
}

void RosterParser::handleCharacterData(const String& data) {
	currentText_ += data;
}

}
