/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
    class SWIFTEN_API MIXParticipantParser : public GenericPayloadTreeParser<MIXParticipant> {
        public:
            MIXParticipantParser() {}
            virtual void handleTree(ParserElement::ref root);
    };
}
