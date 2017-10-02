/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Swiften.h>
#include "EchoPayload.h"

class EchoPayloadParser : public Swift::GenericPayloadParser<EchoPayload> {
    public:
        EchoPayloadParser() : currentDepth(0) {}

        void handleStartElement(
                const std::string& /* element */, const std::string& /* ns */, const AttributeMap&) {
            currentDepth++;
        }

        void handleEndElement(const std::string& /* element */, const std::string& /* ns */) {
            currentDepth--;
            if (currentDepth == 0) {
                getPayloadInternal()->setMessage(currentText);
            }
        }

        void handleCharacterData(const std::string& data) {
            currentText += data;
        }

    private:
        int currentDepth;
        std::string currentText;
};

class EchoPayloadParserFactory : public Swift::GenericPayloadParserFactory<EchoPayloadParser> {
    public:
        EchoPayloadParserFactory() :
            GenericPayloadParserFactory<EchoPayloadParser>("echo", "http://swift.im/echo") {}
};
