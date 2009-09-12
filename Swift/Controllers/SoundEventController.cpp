#include "Swift/Controllers/SoundEventController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/SoundPlayer.h"

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, bool playSounds) {
	eventController_ = eventController;
	soundPlayer_ = soundPlayer;
	playSounds_ = playSounds;
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<MessageEvent> event) {
	if (playSounds_) soundPlayer_->playSound(SoundPlayer::MessageReceived);
}

void SoundEventController::setPlaySounds(bool playSounds) {
	playSounds_ = playSounds;
}

}
