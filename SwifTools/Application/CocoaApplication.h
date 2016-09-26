/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

namespace Swift {
    class CocoaApplication {
        public:
            CocoaApplication();
            ~CocoaApplication();

        private:
            class Private;
            const std::unique_ptr<Private> d;
    };
}
