/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/XMPPEvents/StanzaEvent.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"
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
