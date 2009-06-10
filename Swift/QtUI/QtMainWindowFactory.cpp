#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"
#include "QtTreeWidgetFactory.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {

}

MainWindow* QtMainWindowFactory::createMainWindow() {
	return new QtMainWindow(treeWidgetFactory_);
}

}
