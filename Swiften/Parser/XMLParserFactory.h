/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class XMLParser;
    class XMLParserClient;

    class SWIFTEN_API XMLParserFactory {
        public:
            virtual ~XMLParserFactory();

            virtual XMLParser* createXMLParser(XMLParserClient*) = 0;
    };
}
