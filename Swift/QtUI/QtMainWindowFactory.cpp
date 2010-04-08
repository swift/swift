/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"
#include "Roster/QtTreeWidgetFactory.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {
	lastWindow_ = NULL;
}

MainWindow* QtMainWindowFactory::createMainWindow(UIEventStream* eventStream) {
	if (!lastWindow_) {
		lastWindow_  = new QtMainWindow(eventStream, treeWidgetFactory_);
	}
	return lastWindow_;
}

MainWindow* QtMainWindowFactory::getLastCreatedWindow() {
	assert(lastWindow_);
	return lastWindow_;;
}

}
