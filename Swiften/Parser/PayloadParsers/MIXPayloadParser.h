/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
    class SWIFTEN_API MIXPayloadParser : public GenericPayloadTreeParser<MIXPayload> {
        public:
            MIXPayloadParser() {}
            virtual void handleTree(ParserElement::ref root) SWIFTEN_OVERRIDE;
    };
}
