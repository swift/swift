#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"
#include "Swiften/Settings/SettingsProvider.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class EventController;
	class SoundPlayer;
	class UIEventStream;
	class SoundEventController {
		public:
			SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, SettingsProvider* settings, UIEventStream* uiEvents);
			void setPlaySounds(bool playSounds);
			bool getSoundEnabled() {return playSounds_;};
		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event);
			EventController* eventController_;
			SoundPlayer* soundPlayer_;
			bool playSounds_;
			UIEventStream* uiEvents_;
			SettingsProvider* settings_;
	};
}
