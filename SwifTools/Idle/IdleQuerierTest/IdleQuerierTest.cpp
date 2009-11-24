#include <iostream>

#include "SwifTools/Idle/PlatformIdleQuerier.h"
#include "Swiften/Base/sleep.h"

using namespace Swift;

int main() {
	PlatformIdleQuerier querier;
	while (true) {
		std::cout << "Idle time: " << querier.getIdleTimeSeconds() << std::endl;
		Swift::sleep(1000);
	}

	return 0;
}
