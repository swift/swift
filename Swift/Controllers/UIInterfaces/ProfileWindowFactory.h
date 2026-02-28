/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ProfileWindow.h>

namespace Swift {
    class ProfileWindowFactory {
        public:
            virtual ~ProfileWindowFactory() {}

            virtual ProfileWindow* createProfileWindow() = 0;
    };
}
