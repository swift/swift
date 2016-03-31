/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Dock/Dock.h>

namespace Swift {
    class NullDock : public Dock {
        public:
            NullDock() {}

            virtual void setNumberOfPendingMessages(int) {
            }
    };
}
