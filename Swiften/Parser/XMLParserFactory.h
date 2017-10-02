/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class XMLParser;
    class XMLParserClient;

    class SWIFTEN_API XMLParserFactory {
        public:
            virtual ~XMLParserFactory();

            virtual std::unique_ptr<XMLParser> createXMLParser(XMLParserClient*) = 0;
    };
}
