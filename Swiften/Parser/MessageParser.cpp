/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/MessageParser.h>

#include <boost/optional.hpp>

namespace Swift {

MessageParser::MessageParser(PayloadParserFactoryCollection* factories) :
        GenericStanzaParser<Message>(factories) {
    GenericStanzaParser<Message>::getStanzaGeneric()->setType(Message::Normal);
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
