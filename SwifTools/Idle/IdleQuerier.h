/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class IdleQuerier {
        public:
            virtual ~IdleQuerier();

            virtual int getIdleTimeSeconds() = 0;
    };
}
