/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtSettingsProvider* settings) {
	settings_ = settings;
	lastWindow_ = NULL;
}

MainWindow* QtMainWindowFactory::createMainWindow(UIEventStream* eventStream) {
	lastWindow_  = new QtMainWindow(settings_, eventStream);
	return lastWindow_;
}

MainWindow* QtMainWindowFactory::getLastCreatedWindow() {
	assert(lastWindow_);
	return lastWindow_;;
}

}
