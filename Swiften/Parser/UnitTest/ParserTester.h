/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {
    class XMLParser;

    template<typename ParserType>
    class ParserTester : public XMLParserClient {
        public:
            ParserTester(ParserType* parser) : xmlParser_(PlatformXMLParserFactory().createXMLParser(this)), parser_(parser) {
            }

            bool parse(const std::string& data) {
                return xmlParser_->parse(data);
            }

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
                parser_->handleStartElement(element, ns, attributes);
            }

            virtual void handleEndElement(const std::string& element, const std::string& ns) {
                parser_->handleEndElement(element, ns);
            }

            virtual void handleCharacterData(const std::string& data) {
                parser_->handleCharacterData(data);
            }

        private:
            std::unique_ptr<XMLParser> xmlParser_;
            ParserType* parser_;
    };
}
