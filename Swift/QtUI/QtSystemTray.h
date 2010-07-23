/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/SystemTray.h"

#include <QSystemTrayIcon>

class QIcon;

namespace Swift {
	class QtSystemTray : public QObject, public SystemTray {
		Q_OBJECT
		public:
			QtSystemTray();
			~QtSystemTray();
			void setUnreadMessages(bool some);
			void setStatusType(StatusShow::Type type);
		signals:
			void clicked();
		private slots:
			void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
		private:
			void updateStatusIcon();
			StatusShow::Type statusType_;
			QSystemTrayIcon* trayIcon_;
			QIcon onlineIcon_;
			QIcon awayIcon_;
			QIcon dndIcon_;
			QIcon offlineIcon_;
			QIcon newMessageIcon_;
			bool unreadMessages_;
	};
}
