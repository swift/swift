/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "SwifTools/Dock/Dock.h"

namespace Swift {
	class String;
	class CocoaApplication;

	class MacOSXDock : public Dock {
		public:
			MacOSXDock(CocoaApplication* application);

			void setMessage(const String& label);
	};
}
