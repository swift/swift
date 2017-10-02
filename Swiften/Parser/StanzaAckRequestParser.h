/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StanzaAckRequest.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
    class SWIFTEN_API StanzaAckRequestParser : public GenericElementParser<StanzaAckRequest> {
        public:
            StanzaAckRequestParser() : GenericElementParser<StanzaAckRequest>() {}
    };
}
