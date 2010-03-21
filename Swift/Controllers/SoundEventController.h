#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"

namespace Swift {
	class EventController;
	class SoundPlayer;
	class SoundEventController {
		public:
			SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, bool playSounds);
			void setPlaySounds(bool playSounds);
		private:
			void handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event);
			EventController* eventController_;
			SoundPlayer* soundPlayer_;
			bool playSounds_;
	};
}
