#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"
#include "Roster/QtTreeWidgetFactory.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {
	lastWindow_ = NULL;
}

MainWindow* QtMainWindowFactory::createMainWindow(UIEventStream* eventStream) {
	QtMainWindow* window = new QtMainWindow(eventStream, treeWidgetFactory_);
	lastWindow_ = window;
	return window;
}

MainWindow* QtMainWindowFactory::getLastCreatedWindow() {
	return lastWindow_;
}

}
