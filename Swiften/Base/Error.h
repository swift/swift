/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API Error {
        public:
            Error() {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Error)
            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(Error)
            virtual ~Error();
    };
}
