/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Parser/AttributeMap.h>

namespace Swift {


    /**
     * A parser for XMPP stanza payloads.
     *
     * The parser is event driven: handleStartElement, handleEndElement, and handleCharacterData will be called
     * when the parser detects start and end of XML elements, or character data.
     * After the data for the given top-level element is processed, getPayload() will be called to retrieve the
     * payload.
     */
    class SWIFTEN_API PayloadParser {
        public:
            virtual ~PayloadParser();

            /**
             * Handle the start of an XML element.
             */
            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) = 0;

            /**
             * Handle the end of an XML element.
             */
            virtual void handleEndElement(const std::string& element, const std::string& ns) = 0;

            /**
             * Handle character data.
             */
            virtual void handleCharacterData(const std::string& data) = 0;

            /**
             * Retrieve a pointer to the payload.
             */
            virtual std::shared_ptr<Payload> getPayload() const = 0;
    };
}
