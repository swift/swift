/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class EventController;
	class SystemTray;

	class SystemTrayController {
		public:
		SystemTrayController(EventController* eventController, SystemTray* systemTray);
		private:
			void handleEventQueueLengthChange(int length);

		private:
			EventController* eventController_;
			SystemTray* systemTray_;
	};
}
