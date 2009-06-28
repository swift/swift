#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Events/MessageEvent.h"

namespace Swift {
	class EventController;
	class SoundPlayer;
	class SoundEventController {
		public:
			SoundEventController(EventController* eventController, SoundPlayer* soundPlayer);
		private:
			void handleEventQueueEventAdded(boost::shared_ptr<MessageEvent> event);
			EventController* eventController_;
			SoundPlayer* soundPlayer_;
	};
}
