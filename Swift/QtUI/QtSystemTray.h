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
		signals:
			void clicked();
		private slots:
			void handleIconActivated(QSystemTrayIcon::ActivationReason reason);
		private:
			QSystemTrayIcon* trayIcon_;
			QIcon standardIcon_;
			QIcon newMessageIcon_;
	};
}
