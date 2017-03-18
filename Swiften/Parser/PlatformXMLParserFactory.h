/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/XMLParserFactory.h>

namespace Swift {
    class SWIFTEN_API PlatformXMLParserFactory : public XMLParserFactory {
        public:
            PlatformXMLParserFactory();

            virtual std::unique_ptr<XMLParser> createXMLParser(XMLParserClient*);
    };
}
