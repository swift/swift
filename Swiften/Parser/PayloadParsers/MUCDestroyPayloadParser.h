/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MUCDestroyPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
    class SWIFTEN_API MUCDestroyPayloadParser : public GenericPayloadTreeParser<MUCDestroyPayload> {
        public:
            virtual void handleTree(ParserElement::ref root);
    };
}
