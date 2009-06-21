#include "QtLoginWindowFactory.h"
#include "QtLoginWindow.h"

namespace Swift {

QtLoginWindowFactory::QtLoginWindowFactory(QSplitter* splitter) {
	splitter_ = splitter;
}

LoginWindow* QtLoginWindowFactory::createLoginWindow(const String& defaultJID, const String& defaultPassword, const String& defaultCertificate) {
	QtLoginWindow* window = new QtLoginWindow(defaultJID, defaultPassword, defaultCertificate);
	if (splitter_) {
		splitter_->insertWidget(0, window);
	}
	return window;
}
}
