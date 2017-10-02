/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/URIHandler/URIHandler.h>

namespace Swift {
    class NullURIHandler : public URIHandler {
        public:
            virtual void start() {
            }

            virtual void stop() {
            }
    };
}
