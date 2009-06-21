#ifndef SWIFT_QtMainWindowFactory_H
#define SWIFT_QtMainWindowFactory_H

#include "Swift/Controllers/MainWindowFactory.h"
#include <QSplitter>

namespace Swift {
	class QtTreeWidgetFactory;
	class QtMainWindowFactory : public MainWindowFactory{
		public:
			QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter);
			MainWindow* createMainWindow();
		private:
			QtTreeWidgetFactory *treeWidgetFactory_;
			QSplitter* splitter_;
	};
}

#endif
