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


