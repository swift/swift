/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/URIHandler/URIHandler.h>

namespace Swift {
    class MacOSXURIHandler : public URIHandler {
        public:
            MacOSXURIHandler();
            virtual ~MacOSXURIHandler();

            virtual void start();
            virtual void stop();

        private:
            class Private;
            Private* p;
    };
}
