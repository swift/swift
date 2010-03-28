#include "Swiften/Parser/PayloadParsers/ChatStateParser.h"

namespace Swift {

ChatStateParser::ChatStateParser() : level_(0) {
}

void ChatStateParser::handleStartElement(const String& element, const String&, const AttributeMap&) {
	if (level_ == 0) {
		ChatState::ChatStateType state = ChatState::Active;
		if (element == "active") {
			state = ChatState::Active;
		} else if (element == "composing") {
			state = ChatState::Composing;
		} else if (element == "inactive") {
			state = ChatState::Inactive;
		} else if (element == "paused") {
			state = ChatState::Paused;
		} else if (element == "gone") {
			state = ChatState::Gone;
		}
		getPayloadInternal()->setChatState(state);
	}
	++level_;
}

void ChatStateParser::handleEndElement(const String&, const String&) {
	--level_;
}

void ChatStateParser::handleCharacterData(const String&) {

}

}
