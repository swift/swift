/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
    class PlatformIdleQuerier : public IdleQuerier {
        public:
            PlatformIdleQuerier();
            ~PlatformIdleQuerier();

            virtual int getIdleTimeSeconds() {
                return querier->getIdleTimeSeconds();
            }

        private:
            IdleQuerier* querier;
    };
}
