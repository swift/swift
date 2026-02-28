/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstddef>

namespace Swift {


    class Dock {
        public:
            virtual ~Dock();

            virtual void setNumberOfPendingMessages(size_t i) = 0;
    };
}
