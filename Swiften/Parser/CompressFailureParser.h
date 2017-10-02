/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CompressFailure.h>
#include <Swiften/Parser/GenericElementParser.h>

namespace Swift {
    class SWIFTEN_API CompressFailureParser : public GenericElementParser<CompressFailure> {
        public:
            CompressFailureParser() : GenericElementParser<CompressFailure>() {}
    };
}
