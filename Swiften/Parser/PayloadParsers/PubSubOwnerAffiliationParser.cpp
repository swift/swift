/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubOwnerAffiliationParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/EnumParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

using namespace Swift;

PubSubOwnerAffiliationParser::PubSubOwnerAffiliationParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubOwnerAffiliationParser::~PubSubOwnerAffiliationParser() {
}

void PubSubOwnerAffiliationParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("jid")) {
            if (boost::optional<JID> jid = JID::parse(*attributeValue)) {
                getPayloadInternal()->setJID(*jid);
            }
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("affiliation")) {
            if (boost::optional<PubSubOwnerAffiliation::Type> value = EnumParser<PubSubOwnerAffiliation::Type>()(PubSubOwnerAffiliation::None, "none")(PubSubOwnerAffiliation::Member, "member")(PubSubOwnerAffiliation::Outcast, "outcast")(PubSubOwnerAffiliation::Owner, "owner")(PubSubOwnerAffiliation::Publisher, "publisher")(PubSubOwnerAffiliation::PublishOnly, "publish-only").parse(*attributeValue)) {
                getPayloadInternal()->setType(*value);
            }
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubOwnerAffiliationParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level;
    if (currentPayloadParser) {
        if (level >= 1) {
            currentPayloadParser->handleEndElement(element, ns);
        }

        if (level == 1) {

            currentPayloadParser.reset();
        }
    }
}

void PubSubOwnerAffiliationParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
