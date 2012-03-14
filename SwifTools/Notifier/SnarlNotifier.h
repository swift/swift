/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <SwifTools/Notifier/Notifier.h>
#include <SnarlInterface.h>

namespace Swift {
	class Win32NotifierWindow;

	class SnarlNotifier : public Notifier {
		public:
			SnarlNotifier(const std::string& name, Win32NotifierWindow* window, const boost::filesystem::path& icon);
			~SnarlNotifier();

			virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
			virtual bool isAvailable() const;
		
			virtual void purgeCallbacks() {
				notifications.clear();
			}

		private:
			void handleMessageReceived(MSG* message);

		private:
			Snarl::V41::SnarlInterface snarl;
			Win32NotifierWindow* window;
			bool available;
			typedef std::map<int, boost::function<void()> > NotificationsMap;
			NotificationsMap notifications;
	};
}
