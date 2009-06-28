#include "QtSoundPlayer.h"

#include <phonon/MediaObject>
#include <phonon/AudioOutput>

namespace Swift{
	
QtSoundPlayer::QtSoundPlayer() {
	audioOutput_ = new Phonon::AudioOutput(Phonon::NotificationCategory);
	
	messageReceived_ = new Phonon::MediaObject();
	messageReceived_->setCurrentSource(Phonon::MediaSource(":/sounds/messageReceived.wav"));
	Phonon::Path path = Phonon::createPath(messageReceived_, audioOutput_);
}

QtSoundPlayer::~QtSoundPlayer() {
	delete messageReceived_;
	delete audioOutput_;
}
	
void QtSoundPlayer::playSound(SoundEffect sound) {
	switch (sound) {
		case MessageReceived: 
			messageReceived_->play();
			break;
	}
}

}