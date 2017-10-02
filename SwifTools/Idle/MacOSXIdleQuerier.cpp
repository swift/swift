/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Idle/MacOSXIdleQuerier.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <cassert>
#include <iostream>
#include <boost/numeric/conversion/cast.hpp>
#include <CoreFoundation/CoreFoundation.h>

namespace Swift {

MacOSXIdleQuerier::MacOSXIdleQuerier() : ioService(0) {
    mach_port_t masterPort;
    IOMasterPort(MACH_PORT_NULL, &masterPort);
    ioService = IOServiceGetMatchingService(masterPort, IOServiceMatching("IOHIDSystem"));
    assert(ioService);
}

int MacOSXIdleQuerier::getIdleTimeSeconds() {
    CFTypeRef property = IORegistryEntryCreateCFProperty(ioService, CFSTR("HIDIdleTime"), kCFAllocatorDefault, 0);
    uint64_t idle = 0;
    bool result = CFNumberGetValue((CFNumberRef)property, kCFNumberSInt64Type, &idle);
    assert(result);
    (void) result;
    CFRelease(property);
    return boost::numeric_cast<int>(idle / 1000000000);
}

}
