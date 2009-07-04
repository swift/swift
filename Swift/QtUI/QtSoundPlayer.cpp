#include "QtSoundPlayer.h"

#include <phonon/MediaObject>
#include <phonon/AudioOutput>


namespace Swift{
	
QtSoundPlayer::QtSoundPlayer() {
	audioOutput_ = new Phonon::AudioOutput(Phonon::NotificationCategory);
	
	messageReceived_ = new Phonon::MediaObject();
	messageReceived_->setCurrentSource(Phonon::MediaSource(":/sounds/message-received.wav"));
	Phonon::createPath(messageReceived_, audioOutput_);
	connect(messageReceived_, SIGNAL(finished()), this, SLOT(handleFinished()));
}

QtSoundPlayer::~QtSoundPlayer() {
	delete messageReceived_;
	delete audioOutput_;
}
	
void QtSoundPlayer::playSound(SoundEffect sound) {
	switch (sound) {
		case MessageReceived: 
			//messageReceived_->stop();
			messageReceived_->play();
			break;
	}
}

void QtSoundPlayer::handleFinished() {
	messageReceived_->stop();
}

}