/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>

namespace Swift {
    class SWIFTEN_API DomainNameResolveError : public Error {
        public:
            DomainNameResolveError() {}
    };
}
