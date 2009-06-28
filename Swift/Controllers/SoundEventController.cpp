#include "Swift/Controllers/SoundEventController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/SoundPlayer.h"

namespace Swift {

SoundEventController::SoundEventController(EventController* eventController, SoundPlayer* soundPlayer) {
	eventController_ = eventController;
	soundPlayer_ = soundPlayer;
	eventController_->onEventQueueEventAdded.connect(boost::bind(&SoundEventController::handleEventQueueEventAdded, this, _1));
}

void SoundEventController::handleEventQueueEventAdded(boost::shared_ptr<MessageEvent> event) {
	soundPlayer_->playSound(SoundPlayer::MessageReceived);
}


}
