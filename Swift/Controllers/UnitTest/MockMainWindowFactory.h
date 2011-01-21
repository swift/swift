/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"
#include "Swift/Controllers/UnitTest/MockMainWindow.h"

namespace Swift {

	class MockMainWindowFactory : public MainWindowFactory {
		public:
			MockMainWindowFactory() : last(NULL) {};

			virtual ~MockMainWindowFactory() {};

			/**
			 * Transfers ownership of result.
			 */
			virtual MainWindow* createMainWindow(UIEventStream*) {last = new MockMainWindow();return last;};
			MockMainWindow* last;
	};
}


