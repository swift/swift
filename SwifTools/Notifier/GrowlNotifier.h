/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem/fstream.hpp>

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
	/**
	 * Preconditions for using growlnotifier:
	 * - Must be part a bundle.
	 * - The Carbon/Cocoa application loop must be running (e.g. through QApplication)
	 *   such that notifications are coming through.
	 */
	class GrowlNotifier : public Notifier {
		public:
			GrowlNotifier(const std::string& name);
			~GrowlNotifier();

			virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
			virtual bool isExternallyConfigured() const;

			// Called by the delegate. Don't call.
			void handleNotificationClicked(void* data);
			void handleNotificationTimedOut(void* data);
		
			virtual void purgeCallbacks();

		private:
			void clearPendingNotifications();
		
		private:
			class Private;
			boost::shared_ptr<Private> p;
	};
}
