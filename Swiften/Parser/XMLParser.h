/*
 * Copyright (c) 2010-2019 Isode Limited.
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
            XMLParser(XMLParserClient* client, bool allowComments = false);
            virtual ~XMLParser();

            virtual bool parse(const std::string& data, bool finalData = false) = 0;

            XMLParserClient* getClient() const {
                return client_;
            }

            bool allowsComments() const {
                return allowComments_;
            }

        private:
            XMLParserClient* client_;
            const bool allowComments_ = false;
    };
}
