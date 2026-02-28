/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API Element {
        public:
            Element() {}
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Element)
            virtual ~Element();

            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(Element)
    };
}
