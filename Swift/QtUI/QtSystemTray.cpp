/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma GCC diagnostic ignored "-Wredundant-decls"

#include <Swift/QtUI/QtSystemTray.h>

#include <QtDebug>
#if defined (Q_OS_UNIX) && !defined(Q_OS_MAC)
#include <QDBusInterface>
#endif
#include <QIcon>
#include <QPixmap>
#include <QResource>
#include <QMenu>
#include <QAction>

namespace Swift {
QtSystemTray::QtSystemTray() : QObject(), statusType_(StatusShow::None), trayMenu_(nullptr), onlineIcon_(":icons/online.png"), awayIcon_(":icons/away.png"), dndIcon_(":icons/dnd.png"), offlineIcon_(":icons/offline.png"), newMessageIcon_(":icons/new-chat.png"), throbberMovie_(":/icons/connecting.mng"), unreadMessages_(false), connecting_(false) {
    trayIcon_ = new QSystemTrayIcon(offlineIcon_);
    trayIcon_->setToolTip("Swift");
    connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(&throbberMovie_, SIGNAL(frameChanged(int)), this, SLOT(handleThrobberFrameChanged(int)));
#if defined (Q_OS_UNIX) && !defined(Q_OS_MAC)
    bool isUnity = QDBusInterface("com.canonical.Unity.Launcher", "/com/canonical/Unity/Launcher").isValid();
    if (isUnity) {
        // Add an activation menu, because this is the only way to get the application to the
        // front on Unity. See the README for sni-qt (which handles Qt tray icons for Unity)
        trayMenu_ = new QMenu();
        QAction* showAction = new QAction(QString("Show/Hide"), this);
        connect(showAction, SIGNAL(triggered()), SIGNAL(clicked()));
        trayMenu_->addAction(showAction);
        trayIcon_->setContextMenu(trayMenu_);
    }
#endif
    trayIcon_->show();
}

QtSystemTray::~QtSystemTray() {
    delete trayMenu_;
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
    }    else {
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
