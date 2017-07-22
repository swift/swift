/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXInvitation.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
    class SWIFTEN_API MIXInvitationParser : public GenericPayloadTreeParser<MIXInvitation> {
        public:
            MIXInvitationParser() {}
            virtual void handleTree(ParserElement::ref root) override;
    };
}
