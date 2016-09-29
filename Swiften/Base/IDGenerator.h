/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API IDGenerator {
        public:
            IDGenerator();
            virtual ~IDGenerator();

            virtual std::string generateID();
    };
}
