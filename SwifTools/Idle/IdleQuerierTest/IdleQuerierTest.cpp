/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <cassert>

#include <SwifTools/Idle/PlatformIdleQuerier.h>
#include <Swiften/Base/sleep.h>

using namespace Swift;

int main() {
	PlatformIdleQuerier querier;
	while (true) {
		std::cout << "Idle time: " << querier.getIdleTimeSeconds() << std::endl;
		Swift::sleep(1000);
	}
	assert(false);
	return 0;
}
