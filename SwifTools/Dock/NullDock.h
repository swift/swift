/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstddef>

#include <SwifTools/Dock/Dock.h>

namespace Swift {
    class NullDock : public Dock {
        public:
            NullDock() {}

            virtual void setNumberOfPendingMessages(size_t) {
            }
    };
}
