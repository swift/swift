/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/PubSubAffiliationParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/EnumParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>

using namespace Swift;

PubSubAffiliationParser::PubSubAffiliationParser(PayloadParserFactoryCollection* /*parsers*/) : level(0) {
}

PubSubAffiliationParser::~PubSubAffiliationParser() {
}

void PubSubAffiliationParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level == 0) {
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("node")) {
            getPayloadInternal()->setNode(*attributeValue);
        }
        if (boost::optional<std::string> attributeValue = attributes.getAttributeValue("affiliation")) {
            if (boost::optional<PubSubAffiliation::Type> value = EnumParser<PubSubAffiliation::Type>()(PubSubAffiliation::None, "none")(PubSubAffiliation::Member, "member")(PubSubAffiliation::Outcast, "outcast")(PubSubAffiliation::Owner, "owner")(PubSubAffiliation::Publisher, "publisher")(PubSubAffiliation::PublishOnly, "publish-only").parse(*attributeValue)) {
                getPayloadInternal()->setType(*value);
            }
        }
    }

    if (level >= 1 && currentPayloadParser) {
        currentPayloadParser->handleStartElement(element, ns, attributes);
    }
    ++level;
}

void PubSubAffiliationParser::handleEndElement(const std::string& element, const std::string& ns) {
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

void PubSubAffiliationParser::handleCharacterData(const std::string& data) {
    if (level > 1 && currentPayloadParser) {
        currentPayloadParser->handleCharacterData(data);
    }
}
