/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <QObject>
#include <SwifTools/Notifier/Notifier.h>
#include <SwifTools/Notifier/SnarlNotifier.h>

class QSystemTrayIcon;

namespace Swift {
	class WindowsNotifier : public QObject, public Notifier {
			Q_OBJECT

		public:
			WindowsNotifier(const std::string& name, const boost::filesystem::path& icon, QSystemTrayIcon* tray);
			~WindowsNotifier();

			virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
			virtual void purgeCallbacks();

		private slots:
			void handleMessageClicked();

		private:
			QSystemTrayIcon* tray;
			Win32NotifierWindow* notifierWindow;
			SnarlNotifier* snarlNotifier;
			boost::function<void()> lastCallback;
	};
}
