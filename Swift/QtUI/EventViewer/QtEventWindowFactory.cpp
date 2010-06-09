/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/EventViewer/QtEventWindowFactory.h"

#include "Swift/QtUI/QtMainWindowFactory.h"
#include "Swift/QtUI/QtMainWindow.h"

namespace Swift {

QtEventWindowFactory::QtEventWindowFactory(QtMainWindowFactory* mainWindowFactory) {
	mainWindowFactory_ = mainWindowFactory;
}

EventWindow* QtEventWindowFactory::createEventWindow() {
	return static_cast<QtMainWindow*>(mainWindowFactory_->getLastCreatedWindow())->getEventWindow();
}

}
