/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtSystemTray.h"

#include <QIcon>
#include <QPixmap>
#include <QResource>

namespace Swift {
QtSystemTray::QtSystemTray() : QObject(), standardIcon_(":icons/online.png"), newMessageIcon_(":icons/new-chat.png") {
	trayIcon_ = new QSystemTrayIcon(standardIcon_);
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));
	trayIcon_->show();
}

QtSystemTray::~QtSystemTray() {
	delete trayIcon_;
}

void QtSystemTray::setUnreadMessages(bool some) {
	trayIcon_->setIcon(some ? newMessageIcon_ : standardIcon_);
}

void QtSystemTray::handleIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		emit clicked();
	}
}
}
