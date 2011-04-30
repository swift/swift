/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>

#include <SwifTools/Idle/PlatformIdleQuerier.h>
#include <Swiften/Base/sleep.h>

using namespace Swift;

int main() {
	PlatformIdleQuerier querier;
	while (true) {
		std::cout << "Idle time: " << querier.getIdleTimeSeconds() << std::endl;
		Swift::sleep(1000);
	}

	return 0;
}
