/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
    class DummyIdleQuerier : public IdleQuerier {
        public:
            DummyIdleQuerier() {}

            virtual int getIdleTimeSeconds() {
                return 0;
            }
    };
}
