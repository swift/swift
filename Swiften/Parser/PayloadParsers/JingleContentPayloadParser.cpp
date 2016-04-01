/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/JingleContentPayloadParser.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>

namespace Swift {
    JingleContentPayloadParser::JingleContentPayloadParser(PayloadParserFactoryCollection* factories) : factories(factories), level(0) {

    }

    void JingleContentPayloadParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
        if (level == 0) {
            std::string creator = attributes.getAttributeValue("creator").get_value_or("");
            if (creator == "initiator") {
                getPayloadInternal()->setCreator(JingleContentPayload::InitiatorCreator);
            } else if (creator == "responder") {
                getPayloadInternal()->setCreator(JingleContentPayload::ResponderCreator);
            } else {
                getPayloadInternal()->setCreator(JingleContentPayload::UnknownCreator);
            }

            getPayloadInternal()->setName(attributes.getAttributeValue("name").get_value_or(""));
        }

        if (level == 1) {
            PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
            if (payloadParserFactory) {
                currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
            }
        }

        if (level >= 1 && currentPayloadParser) {
            currentPayloadParser->handleStartElement(element, ns, attributes);
        }

        ++level;
    }

    void JingleContentPayloadParser::handleEndElement(const std::string& element, const std::string& ns) {
        --level;

        if (currentPayloadParser) {
            if (level >= 1) {
                currentPayloadParser->handleEndElement(element, ns);
            }

            if (level == 1) {
                std::shared_ptr<JingleTransportPayload> transport = std::dynamic_pointer_cast<JingleTransportPayload>(currentPayloadParser->getPayload());
                if (transport) {
                    getPayloadInternal()->addTransport(transport);
                }

                std::shared_ptr<JingleDescription> description = std::dynamic_pointer_cast<JingleDescription>(currentPayloadParser->getPayload());
                if (description) {
                    getPayloadInternal()->addDescription(description);
                }
            }
        }
    }

    void JingleContentPayloadParser::handleCharacterData(const std::string& data) {
        if (level > 1 && currentPayloadParser) {
            currentPayloadParser->handleCharacterData(data);
        }
    }
}
