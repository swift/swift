#include "Swift/Controllers/SoundEventController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/SoundPlayer.h"

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer, bool playSounds) {
	eventController_ = eventController;
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
	soundPlayer_ = soundPlayer;
	playSounds_ = playSounds;
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<Event>) {
	if (playSounds_) soundPlayer_->playSound(SoundPlayer::MessageReceived);
}

void SoundEventController::setPlaySounds(bool playSounds) {
	playSounds_ = playSounds;
}

}
