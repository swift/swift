/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
    class AdHocCommandWindow {
        public:
            virtual ~AdHocCommandWindow() {}
            virtual void setOnline(bool /*online*/) {}
            boost::signal<void ()> onClosing;
    };
}
