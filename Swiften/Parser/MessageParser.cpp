#include <iostream>

#include "Swiften/Parser/MessageParser.h"

namespace Swift {

MessageParser::MessageParser(PayloadParserFactoryCollection* factories) : 
		GenericStanzaParser<Message>(factories) {
	getStanzaGeneric()->setType(Message::Normal);
}

void MessageParser::handleStanzaAttributes(const AttributeMap& attributes) {
	AttributeMap::const_iterator type = attributes.find("type");
	if (type != attributes.end()) {
		if (type->second == "chat") {
			getStanzaGeneric()->setType(Message::Chat);
		}
		else if (type->second == "error") {
			getStanzaGeneric()->setType(Message::Error);
		}
		else if (type->second == "groupchat") {
			getStanzaGeneric()->setType(Message::Groupchat);
		}
		else if (type->second == "headline") {
			getStanzaGeneric()->setType(Message::Headline);
		}
		else {
			getStanzaGeneric()->setType(Message::Normal);
		}
	}
}

}
