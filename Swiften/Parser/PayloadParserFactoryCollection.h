/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
    class PayloadParserFactory;

    class SWIFTEN_API PayloadParserFactoryCollection {
        public:
            PayloadParserFactoryCollection();
            virtual ~PayloadParserFactoryCollection();

            void addFactory(PayloadParserFactory* factory);
            void removeFactory(PayloadParserFactory* factory);
            void setDefaultFactory(PayloadParserFactory* factory);

            PayloadParserFactory* getPayloadParserFactory(const std::string& element, const std::string& ns, const AttributeMap& attributes);

        private:
            std::vector<PayloadParserFactory*> factories_;
            PayloadParserFactory* defaultFactory_;
    };
}
