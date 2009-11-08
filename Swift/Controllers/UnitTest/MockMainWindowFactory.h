#pragma once

#include "Swift/Controllers/MainWindowFactory.h"
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
			virtual MainWindow* createMainWindow() {return new MockMainWindow(factory_->createTreeWidget());};
	private:
			TreeWidgetFactory* factory_;
	};
}


