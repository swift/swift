/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Idle/MacOSXIdleQuerier.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <cassert>
#include <iostream>
#include <CoreFoundation/CoreFoundation.h>

namespace Swift {

MacOSXIdleQuerier::MacOSXIdleQuerier() : ioService(NULL) {
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
	return idle / 1000000000;
}

}
