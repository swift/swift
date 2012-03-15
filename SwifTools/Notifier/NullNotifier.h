/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/Notifier/Notifier.h>

namespace Swift {
	class NullNotifier : public Notifier {
		public:
			virtual void showMessage(Type, const std::string&, const std::string&, const boost::filesystem::path&, boost::function<void()>) {
			}
			virtual void purgeCallbacks() {
			}
	};
}
