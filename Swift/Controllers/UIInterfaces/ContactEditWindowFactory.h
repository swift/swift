/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>

namespace Swift {
    class ContactEditWindowFactory {
        public:
            virtual ~ContactEditWindowFactory() {}

            virtual ContactEditWindow* createContactEditWindow() = 0;
    };
}
