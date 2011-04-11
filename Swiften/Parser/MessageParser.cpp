/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/optional.hpp>

#include <Swiften/Parser/MessageParser.h>

namespace Swift {

MessageParser::MessageParser(PayloadParserFactoryCollection* factories) : 
		GenericStanzaParser<Message>(factories) {
	getStanzaGeneric()->setType(Message::Normal);
}

void MessageParser::handleStanzaAttributes(const AttributeMap& attributes) {
	boost::optional<std::string> type = attributes.getAttributeValue("type");
	if (type) {
		if (*type == "chat") {
			getStanzaGeneric()->setType(Message::Chat);
		}
		else if (*type == "error") {
			getStanzaGeneric()->setType(Message::Error);
		}
		else if (*type == "groupchat") {
			getStanzaGeneric()->setType(Message::Groupchat);
		}
		else if (*type == "headline") {
			getStanzaGeneric()->setType(Message::Headline);
		}
		else {
			getStanzaGeneric()->setType(Message::Normal);
		}
	}
}

}
