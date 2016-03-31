/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QMovie>
#include <QSystemTrayIcon>

#include <Swift/Controllers/SystemTray.h>

class QIcon;
class QMenu;

namespace Swift {

    class QtSystemTray : public QObject, public SystemTray {
        Q_OBJECT
        public:
            QtSystemTray();
            ~QtSystemTray();
            void setUnreadMessages(bool some);
            void setStatusType(StatusShow::Type type);
            void setConnecting();
            QSystemTrayIcon* getQSystemTrayIcon() {
                return trayIcon_;
            }

        signals:
            void clicked();
        private slots:
            void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
            void handleThrobberFrameChanged(int);
        private:
            void updateStatusIcon();
            StatusShow::Type statusType_;
            QSystemTrayIcon* trayIcon_;
            QMenu* trayMenu_;
            QIcon onlineIcon_;
            QIcon awayIcon_;
            QIcon dndIcon_;
            QIcon offlineIcon_;
            QIcon newMessageIcon_;
            QMovie throbberMovie_;
            bool unreadMessages_;
            bool connecting_;
    };
}
