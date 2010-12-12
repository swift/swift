/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"

namespace Swift {
	class QtTreeWidgetFactory;
	class QtSettingsProvider;

	class QtMainWindowFactory : public MainWindowFactory{
		public:
			QtMainWindowFactory(QtSettingsProvider* settings);
			MainWindow* createMainWindow(UIEventStream* eventStream);
			MainWindow* getLastCreatedWindow();
		private:
			QtSettingsProvider* settings_;
			MainWindow* lastWindow_;
	};
}
