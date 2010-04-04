#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class EventController;
	class SoundPlayer;
	class UIEventStream;
	class SoundEventController {
		public:
			SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, bool playSounds, UIEventStream* uiEvents);
			void setPlaySounds(bool playSounds);
		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event);
			EventController* eventController_;
			SoundPlayer* soundPlayer_;
			bool playSounds_;
			UIEventStream* uiEvents_;
	};
}
