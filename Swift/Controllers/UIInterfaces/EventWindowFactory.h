/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class EventWindow;

	class EventWindowFactory {
		public:
			virtual ~EventWindowFactory() {};
			/**
			 * Transfers ownership of result.
			 */
			virtual EventWindow* createEventWindow() = 0;

	};
}


