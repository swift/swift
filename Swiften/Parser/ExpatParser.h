/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/XMLParser.h>

namespace Swift {
    class SWIFTEN_API ExpatParser : public XMLParser, public boost::noncopyable {
        public:
            ExpatParser(XMLParserClient* client);
            ~ExpatParser();

            bool parse(const std::string& data);

            void stopParser();

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
