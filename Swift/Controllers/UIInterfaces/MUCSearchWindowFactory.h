/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/MUCSearchWindow.h>

namespace Swift {
    class UIEventStream;
    class MUCSearchWindowFactory {
        public:
            virtual ~MUCSearchWindowFactory() {}

            virtual MUCSearchWindow* createMUCSearchWindow() = 0;
    };
}
