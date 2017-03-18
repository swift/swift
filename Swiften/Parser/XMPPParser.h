/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/AttributeMap.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {
    class XMLParser;
    class XMPPParserClient;
    class XMLParserFactory;
    class ElementParser;
    class PayloadParserFactoryCollection;

    class SWIFTEN_API XMPPParser : public XMLParserClient, boost::noncopyable {
        public:
            XMPPParser(
                    XMPPParserClient* parserClient,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    XMLParserFactory* xmlParserFactory);
            virtual ~XMPPParser();

            bool parse(const std::string&);

        private:
            virtual void handleStartElement(
                    const std::string& element,
                    const std::string& ns,
                    const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string& ns);
            virtual void handleCharacterData(const std::string& data);

            ElementParser* createElementParser(const std::string& element, const std::string& xmlns);

        private:
            std::unique_ptr<XMLParser> xmlParser_;
            XMPPParserClient* client_;
            PayloadParserFactoryCollection* payloadParserFactories_;
            enum Level {
                TopLevel = 0,
                StreamLevel = 1,
                ElementLevel = 2
            };
            int level_;
            ElementParser* currentElementParser_;
            bool parseErrorOccurred_;
    };
}
