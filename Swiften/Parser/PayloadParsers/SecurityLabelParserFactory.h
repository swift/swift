/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/SecurityLabelParser.h>

namespace Swift {
    class SWIFTEN_API SecurityLabelParserFactory : public GenericPayloadParserFactory<SecurityLabelParser> {
        public:
            SecurityLabelParserFactory() : GenericPayloadParserFactory<SecurityLabelParser>("securitylabel", "urn:xmpp:sec-label:0") {}
    };
}
