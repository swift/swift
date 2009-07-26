#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"
#include "Roster/QtTreeWidgetFactory.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {

}

MainWindow* QtMainWindowFactory::createMainWindow() {
	QtMainWindow* window = new QtMainWindow(treeWidgetFactory_);
	return window;
}

}
