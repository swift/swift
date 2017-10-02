/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class XMLParserClient;

    class SWIFTEN_API XMLParser {
        public:
            XMLParser(XMLParserClient* client);
            virtual ~XMLParser();

            virtual bool parse(const std::string& data) = 0;

            XMLParserClient* getClient() const {
                return client_;
            }

        private:
            XMLParserClient* client_;
    };
}
