/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/EventLoop/SimpleEventLoop.h>
#include "Slimber/CLI/DummyMenulet.h"
#include "Slimber/MainController.h"

using namespace Swift;

int main() {
	SimpleEventLoop eventLoop;
	
	DummyMenulet menulet;
	MainController controller(&menulet, &eventLoop);

	eventLoop.run();
	return 0;
}
