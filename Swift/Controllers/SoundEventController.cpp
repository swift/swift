#include "Swift/Controllers/SoundEventController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/SoundPlayer.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/ToggleSoundsUIEvent.h"

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, bool playSounds, UIEventStream* uiEvents) {
	eventController_ = eventController;
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
	soundPlayer_ = soundPlayer;
	playSounds_ = playSounds;
	uiEvents_ = uiEvents;
	uiEvents_->onUIEvent.connect(boost::bind(&SoundEventController::handleUIEvent, this, _1));
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent>) {
	if (playSounds_) soundPlayer_->playSound(SoundPlayer::MessageReceived);
}

void SoundEventController::setPlaySounds(bool playSounds) {
	playSounds_ = playSounds;
}

void SoundEventController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<ToggleSoundsUIEvent> soundEvent = boost::dynamic_pointer_cast<ToggleSoundsUIEvent>(event);
	if (soundEvent) {
		setPlaySounds(soundEvent->getEnabled());
	}
}

}
