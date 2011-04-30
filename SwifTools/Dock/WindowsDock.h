/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QSystemTrayIcon>
#include <boost/lexical_cast.hpp>

#include <SwifTools/Dock/Dock.h>
#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
	class WindowsDock : public Dock {
		public:
			WindowsDock(QSystemTrayIcon* tray, Notifier* notifier) : tray(tray), notifier(notifier) {}

			virtual void setNumberOfPendingMessages(int i) {
				if (notifier->isAvailable()) {
					return;
				}

				if (i > 0) {
					std::string message = boost::lexical_cast<std::string>(i) + " new message";
					if (i > 1) {
						message += "s";
					}
					message += " received.";
					tray->showMessage("New messages", message.c_str(), QSystemTrayIcon::NoIcon);
				}
				else {
					tray->showMessage("", "", QSystemTrayIcon::NoIcon, 0);
				}
			}

		private:
			QSystemTrayIcon* tray;
			Notifier* notifier;
	};
}
