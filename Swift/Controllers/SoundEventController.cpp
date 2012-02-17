/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/SoundEventController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/SoundPlayer.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/SettingConstants.h>

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, SettingsProvider* settings) {
	settings_ = settings;
	eventController_ = eventController;
	soundPlayer_ = soundPlayer;
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
	settings_->onSettingChanged.connect(boost::bind(&SoundEventController::handleSettingChanged, this, _1));

	playSounds_ = settings->getSetting(SettingConstants::PLAY_SOUNDS);
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event) {
	if (playSounds_ && !event->getConcluded()) {
		soundPlayer_->playSound(SoundPlayer::MessageReceived);
	}
}

void SoundEventController::setPlaySounds(bool playSounds) {
	playSounds_ = playSounds;
	settings_->storeSetting(SettingConstants::PLAY_SOUNDS, playSounds);
}

void SoundEventController::handleSettingChanged(const std::string& settingPath) {
	if (SettingConstants::PLAY_SOUNDS.getKey() == settingPath) {
		playSounds_ = settings_->getSetting(SettingConstants::PLAY_SOUNDS);
	}
}

}
