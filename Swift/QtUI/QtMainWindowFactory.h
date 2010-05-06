/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtMainWindowFactory_H
#define SWIFT_QtMainWindowFactory_H

#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"

namespace Swift {
	class QtTreeWidgetFactory;
	class QtMainWindowFactory : public MainWindowFactory{
		public:
			QtMainWindowFactory();
			MainWindow* createMainWindow(UIEventStream* eventStream);
			MainWindow* getLastCreatedWindow();
		private:
			MainWindow* lastWindow_;
	};
}

#endif
