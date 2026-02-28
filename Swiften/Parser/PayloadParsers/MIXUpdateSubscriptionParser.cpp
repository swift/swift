/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXUpdateSubscriptionParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

MIXUpdateSubscriptionParser::MIXUpdateSubscriptionParser() : level_(0) {
}

MIXUpdateSubscriptionParser::~MIXUpdateSubscriptionParser() {
}

void MIXUpdateSubscriptionParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level_ == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
    }

    if (level_ == 1) {
        if (element == "subscribe" && ns == "urn:xmpp:mix:0") {
            if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
                getPayloadInternal()->addSubscription(*attributeValue);
            }
        }
    }

    ++level_;
}

void MIXUpdateSubscriptionParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
}

void MIXUpdateSubscriptionParser::handleCharacterData(const std::string&) {
}

}
