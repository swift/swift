/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <IOKit/IOKitLib.h>

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
    class MacOSXIdleQuerier : public IdleQuerier {
        public:
            MacOSXIdleQuerier();

            virtual int getIdleTimeSeconds();

        private:
            io_service_t ioService;
    };
}
