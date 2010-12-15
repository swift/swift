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
QtSystemTray::QtSystemTray() : QObject(), onlineIcon_(":icons/online.png"), awayIcon_(":icons/away.png"), dndIcon_(":icons/dnd.png"), offlineIcon_(":icons/offline.png"), newMessageIcon_(":icons/new-chat.png"), throbberMovie_(":/icons/connecting.mng"), unreadMessages_(false), connecting_(false) {
	trayIcon_ = new QSystemTrayIcon(offlineIcon_);
	trayIcon_->setToolTip("Swift");
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));
	connect(&throbberMovie_, SIGNAL(frameChanged(int)), this, SLOT(handleThrobberFrameChanged(int)));
	trayIcon_->show();
}

QtSystemTray::~QtSystemTray() {
	delete trayIcon_;
}

void QtSystemTray::setUnreadMessages(bool some) {
	unreadMessages_ = some;
	updateStatusIcon();
}

void QtSystemTray::handleThrobberFrameChanged(int /*frameNumber*/) {
	trayIcon_->setIcon(QIcon(throbberMovie_.currentPixmap()));
}

void QtSystemTray::setConnecting() {
	connecting_ = true;
	updateStatusIcon();
}

void QtSystemTray::handleIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		emit clicked();
	}
}
void QtSystemTray::setStatusType(StatusShow::Type type) {
	connecting_ = false;
	statusType_ = type;
	updateStatusIcon();
}

void QtSystemTray::updateStatusIcon() {
	throbberMovie_.stop();
	if (unreadMessages_) {
		trayIcon_->setIcon(newMessageIcon_);
	} else if (connecting_) {
		throbberMovie_.start();
	}	else {
		switch (statusType_) {
			case StatusShow::Online : trayIcon_->setIcon(onlineIcon_);break;
			case StatusShow::FFC : trayIcon_->setIcon(onlineIcon_);break;
			case StatusShow::Away : trayIcon_->setIcon(awayIcon_);break;
			case StatusShow::XA : trayIcon_->setIcon(awayIcon_);break;
			case StatusShow::DND : trayIcon_->setIcon(dndIcon_);break;
			case StatusShow::None : trayIcon_->setIcon(offlineIcon_);break;
		}
	}
}

}
