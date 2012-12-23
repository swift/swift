/*
 * Copyright (c) 2010 -2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "SwifTools/Notifier/Notifier.h"
#include "QtCachedImageScaler.h"

namespace Swift {
	class FreeDesktopNotifier : public Notifier {
		public:
			FreeDesktopNotifier(const std::string& name);

			virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback);
			virtual void purgeCallbacks() {}

		private:
			std::string applicationName;
			QtCachedImageScaler imageScaler;
	};
}
