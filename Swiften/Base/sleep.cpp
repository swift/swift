/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/sleep.h>

#include <chrono>
#include <thread>

namespace Swift {

void sleep(unsigned int msecs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

}
