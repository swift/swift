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
