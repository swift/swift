#include "Swift/QtUI/QtSystemTray.h"

#include <QIcon>
#include <QPixmap>
#include <QResource>

namespace Swift {
QtSystemTray::QtSystemTray() : standardIcon_(":icons/tray-standard.png"), newMessageIcon_(":icons/new-chat.png") {
	trayIcon_ = new QSystemTrayIcon(standardIcon_);
	trayIcon_->show();
}

QtSystemTray::~QtSystemTray() {
	delete trayIcon_;
}

void QtSystemTray::setUnreadMessages(bool some) {
	trayIcon_->setIcon(some ? newMessageIcon_ : standardIcon_);
}

}
