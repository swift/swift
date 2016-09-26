/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>

#include <Swiften/Parser/XMLParser.h>

namespace Swift {
    /**
     * Warning: This constructor is not thread-safe, because it depends on global state to
     * check whether it is initialized.
     */
    class LibXMLParser : public XMLParser, public boost::noncopyable {
        public:
            LibXMLParser(XMLParserClient* client);
            virtual ~LibXMLParser();

            bool parse(const std::string& data);

        private:
            static bool initialized;

            struct Private;
            const std::unique_ptr<Private> p;
    };
}
