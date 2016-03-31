/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Idle/XSSIdleQuerier.h>

#include <cassert>
#include <iostream>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

XSSIdleQuerier::XSSIdleQuerier() : display(NULL), info(NULL) {
    display = XOpenDisplay(NULL);
    assert(display);
    rootWindow = DefaultRootWindow(display);
    int event, error;
    available = XScreenSaverQueryExtension(display, &event, &error);
    if (available) {
        info = XScreenSaverAllocInfo();
    }
    else {
        std::cerr << "Warning: XScreenSaver extension not found. Idle time detection will not work." << std::endl;
    }
}

XSSIdleQuerier::~XSSIdleQuerier() {
    XFree(info);
}

int XSSIdleQuerier::getIdleTimeSeconds() {
    if (available) {
        XScreenSaverQueryInfo(display, rootWindow, info);
        return info->idle / 1000;
    }
    else {
        return 0;
    }
}

}
