/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

//#include <windows.h>

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class Win32NotifierWindow {
		public:
			virtual ~Win32NotifierWindow() {}

			virtual HWND getID() const = 0;

			boost::signal<void (MSG*)> onMessageReceived;
	};
}
