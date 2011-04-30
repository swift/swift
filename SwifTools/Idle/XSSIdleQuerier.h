/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

#include <SwifTools/Idle/IdleQuerier.h>

namespace Swift {
	class XSSIdleQuerier : public IdleQuerier {
		public:
			XSSIdleQuerier();
			~XSSIdleQuerier();

			virtual int getIdleTimeSeconds();

		private:
			Display* display;
			Window rootWindow;
			bool available;
			XScreenSaverInfo* info;
	};
}
