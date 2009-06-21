#include "QtMainWindowFactory.h"
#include "QtMainWindow.h"
#include "QtTreeWidgetFactory.h"

namespace Swift {

QtMainWindowFactory::QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter) : treeWidgetFactory_(treeWidgetFactory) {
	splitter_ = splitter;
}

MainWindow* QtMainWindowFactory::createMainWindow() {
	QtMainWindow* window = new QtMainWindow(treeWidgetFactory_);
	if (splitter_) {
		splitter_->addWidget(window);
	}
	return window;
}

}
