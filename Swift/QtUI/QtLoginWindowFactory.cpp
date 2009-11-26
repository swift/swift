#include "QtLoginWindowFactory.h"
#include "QtLoginWindow.h"

#include <QDesktopWidget>

namespace Swift {

QtLoginWindowFactory::QtLoginWindowFactory(QSplitter* splitter, QtSystemTray* systemTray, QtSettingsProvider* settings) : QObject() {
	settings_ = settings;
	splitter_ = splitter;
	systemTray_ = systemTray;
}

LoginWindow* QtLoginWindowFactory::createLoginWindow(UIEventStream* uiEventStream) {
	window_ = new QtLoginWindow(uiEventStream);
	if (splitter_) {
		splitter_->insertWidget(0, window_);
		return window_;
	}
	connect(systemTray_, SIGNAL(clicked()), window_, SLOT(bringToFront()));
	QVariant loginWindowGeometryVariant = settings_->getQSettings()->value("loginWindowGeometry");
	if (loginWindowGeometryVariant.isValid()) {
		window_->restoreGeometry(loginWindowGeometryVariant.toByteArray());
	}
	connect(window_, SIGNAL(geometryChanged()), this, SLOT(handleWindowGeometryChanged()));
	return window_;
}

void QtLoginWindowFactory::handleWindowGeometryChanged() {
	settings_->getQSettings()->setValue("loginWindowGeometry", window_->saveGeometry());
}

}
