/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/CarbonsReceivedParser.h>

namespace Swift {
    CarbonsReceivedParser::CarbonsReceivedParser(PayloadParserFactoryCollection* factories) : GenericPayloadParser<CarbonsReceived>(), factories_(factories), level_(TopLevel) {
    }

    CarbonsReceivedParser::~CarbonsReceivedParser() {
    }

    void CarbonsReceivedParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
        if (level_ == PayloadLevel) {
            if (element == "forwarded") {
                forwardedParser_ = std::dynamic_pointer_cast<ForwardedParser>(std::make_shared<ForwardedParser>(factories_));
            }
        }
        if (forwardedParser_) {
            forwardedParser_->handleStartElement(element, ns, attributes);
        }
        ++level_;
    }

    void CarbonsReceivedParser::handleEndElement(const std::string& element, const std::string& ns) {
        --level_;
        if (forwardedParser_ && level_ >= PayloadLevel) {
            forwardedParser_->handleEndElement(element, ns);
        }
        if (forwardedParser_ && level_ == PayloadLevel) {
            /* done parsing nested stanza */
            getPayloadInternal()->setForwarded(forwardedParser_->getPayloadInternal());
            forwardedParser_.reset();
        }
    }

    void CarbonsReceivedParser::handleCharacterData(const std::string& data) {
        if (forwardedParser_) {
            forwardedParser_->handleCharacterData(data);
        }
    }
}
