/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

namespace Swift {
    class AdHocCommandWindow {
        public:
            virtual ~AdHocCommandWindow() {}
            virtual void setOnline(bool /*online*/) {}
            boost::signals2::signal<void ()> onClosing;
    };
}
