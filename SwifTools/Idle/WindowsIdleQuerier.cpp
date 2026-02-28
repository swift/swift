/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Idle/WindowsIdleQuerier.h>

#include <windows.h>

namespace Swift {

WindowsIdleQuerier::WindowsIdleQuerier() {
}

int WindowsIdleQuerier::getIdleTimeSeconds() {
    LASTINPUTINFO info;
    info.cbSize = sizeof(info);
    if (GetLastInputInfo(&info)) {
        return (GetTickCount() - info.dwTime) / 1000;
    }
    else {
        return 0;
    }
}

}
