/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/SoundEventController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/SoundPlayer.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/ToggleSoundsUIEvent.h"

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, SettingsProvider* settings, UIEventStream* uiEvents) {
	uiEvents_ = uiEvents;
	settings_ = settings;
	eventController_ = eventController;
	soundPlayer_ = soundPlayer;
	uiEvents_->onUIEvent.connect(boost::bind(&SoundEventController::handleUIEvent, this, _1));
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));

	bool playSounds = settings->getBoolSetting("playSounds", true);
	playSounds_ = !playSounds;
	setPlaySounds(playSounds);	
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event) {
	if (playSounds_ && !event->getConcluded()) {
		soundPlayer_->playSound(SoundPlayer::MessageReceived);
	}
}

void SoundEventController::setPlaySounds(bool playSounds) {
	bool transmit = playSounds != playSounds_;
	playSounds_ = playSounds;
	settings_->storeBool("playSounds", playSounds);
	if (transmit) {
		uiEvents_->send(boost::shared_ptr<ToggleSoundsUIEvent>(new ToggleSoundsUIEvent(playSounds_)));
	}
}

void SoundEventController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<ToggleSoundsUIEvent> soundEvent = boost::dynamic_pointer_cast<ToggleSoundsUIEvent>(event);
	if (soundEvent) {
		setPlaySounds(soundEvent->getEnabled());
	}
}

}
