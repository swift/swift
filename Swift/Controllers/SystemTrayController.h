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
