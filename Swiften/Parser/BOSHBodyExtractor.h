/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {
    class XMLParserFactory;

    class SWIFTEN_API BOSHBodyExtractor {
            friend class BOSHBodyParserClient;
        public:
            struct BOSHBody {
                AttributeMap attributes;
                std::string content;
            };

            BOSHBodyExtractor(XMLParserFactory* parserFactory, const ByteArray& data);

            const boost::optional<BOSHBody>& getBody() const {
                return body;
            }

        private:
            boost::optional<BOSHBody> body;
    };
}
