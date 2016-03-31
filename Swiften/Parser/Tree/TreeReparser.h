/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>
#include <Swiften/Parser/PayloadParsers/MUCItemParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class SWIFTEN_API TreeReparser {
        public:
            static boost::shared_ptr<Payload> parseTree(ParserElement::ref root, PayloadParserFactoryCollection* collection);

    };
}
