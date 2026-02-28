/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

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
            const std::unique_ptr<Private> p;
    };
}
