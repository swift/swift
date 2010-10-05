/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "SwifTools/Notifier/Notifier.h"

namespace Swift {
	class FreeDesktopNotifier : public Notifier {
		public:
			FreeDesktopNotifier(const String& name);

			virtual void doShowMessage(Type type, const String& subject, const String& description, const boost::filesystem::path& picture, boost::function<void()> callback);
		
		private:
			String applicationName;
	};
}
