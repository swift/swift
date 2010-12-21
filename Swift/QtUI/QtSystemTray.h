/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/SystemTray.h"

#include <QSystemTrayIcon>
#include <QMovie>

class QIcon;

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
