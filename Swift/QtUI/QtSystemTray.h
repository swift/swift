#pragma once

#include "Swift/Controllers/SystemTray.h"

#include <QSystemTrayIcon>

class QIcon;

namespace Swift {
	class QtSystemTray : public SystemTray {
		public:
			QtSystemTray();
			~QtSystemTray();
			void setUnreadMessages(bool some);
		private:
			QSystemTrayIcon* trayIcon_;
			QIcon standardIcon_;
			QIcon newMessageIcon_;
	};
}
