#include "QtLoginWindowFactory.h"
#include "QtLoginWindow.h"

namespace Swift {

QtLoginWindowFactory::QtLoginWindowFactory(QSplitter* splitter, QtSystemTray* systemTray) : QObject() {
	splitter_ = splitter;
	systemTray_ = systemTray;
}

LoginWindow* QtLoginWindowFactory::createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) {
	QtLoginWindow* window = new QtLoginWindow(defaultJID, defaultPassword, defaultCertificate);
	if (splitter_) {
		splitter_->insertWidget(0, window);
	}
	connect(systemTray_, SIGNAL(clicked()), window, SLOT(show()));
	return window;
}
}
