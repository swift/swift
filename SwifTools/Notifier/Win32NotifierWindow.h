/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
