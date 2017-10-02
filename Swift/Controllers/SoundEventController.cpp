/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/SoundEventController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/Highlighting/HighlightManager.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/SoundPlayer.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, SettingsProvider* settings, HighlightManager* highlightManager) {
    settings_ = settings;
    eventController_ = eventController;
    soundPlayer_ = soundPlayer;
    eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
    highlightManager_ = highlightManager;
    highlightManager_->onHighlight.connect(boost::bind(&SoundEventController::handleHighlight, this, _1));

    settings_->onSettingChanged.connect(boost::bind(&SoundEventController::handleSettingChanged, this, _1));

    playSounds_ = settings->getSetting(SettingConstants::PLAY_SOUNDS);
}

void SoundEventController::handleEventQueueEventAdded(std::shared_ptr<StanzaEvent> event) {
    if (playSounds_ && std::dynamic_pointer_cast<IncomingFileTransferEvent>(event)) {
        soundPlayer_->playSound(SoundPlayer::MessageReceived, "");
    }
}

void SoundEventController::handleHighlight(const HighlightAction& action) {
    if (playSounds_ && action.getSoundFilePath()) {
        soundPlayer_->playSound(SoundPlayer::MessageReceived, action.getSoundFilePath().get_value_or(""));
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
