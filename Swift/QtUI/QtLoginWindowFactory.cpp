#include "QtLoginWindowFactory.h"
#include "QtLoginWindow.h"

namespace Swift {

QtLoginWindowFactory::QtLoginWindowFactory(QSplitter* splitter, QtSystemTray* systemTray) : QObject() {
	splitter_ = splitter;
	systemTray_ = systemTray;
}

LoginWindow* QtLoginWindowFactory::createLoginWindow() {
	QtLoginWindow* window = new QtLoginWindow();
	if (splitter_) {
		splitter_->insertWidget(0, window);
	}
	connect(systemTray_, SIGNAL(clicked()), window, SLOT(bringToFront()));
	return window;
}
}
