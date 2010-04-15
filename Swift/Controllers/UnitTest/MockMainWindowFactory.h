/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swift/Controllers/UnitTest/MockMainWindow.h"

namespace Swift {

	class MockMainWindowFactory : public MainWindowFactory {
		public:
			MockMainWindowFactory(TreeWidgetFactory* treeWidgetFactory) {factory_ = treeWidgetFactory;};
			virtual ~MockMainWindowFactory() {};
			/**
			 * Transfers ownership of result.
			 */
			virtual MainWindow* createMainWindow(UIEventStream*) {return new MockMainWindow(factory_->createTreeWidget());};
	private:
			TreeWidgetFactory* factory_;
	};
}


