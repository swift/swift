/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

//#include <windows.h>

#include <boost/signals2.hpp>

namespace Swift {
    class Win32NotifierWindow {
        public:
            virtual ~Win32NotifierWindow() {}

            virtual HWND getID() const = 0;

            boost::signals2::signal<void (MSG*)> onMessageReceived;
    };
}
