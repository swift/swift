#include "QtSoundPlayer.h"

#include <QSound>

namespace Swift {
	
QtSoundPlayer::QtSoundPlayer() {
}

void QtSoundPlayer::playSound(SoundEffect sound) {
	switch (sound) {
		case MessageReceived: 
			QSound::play(":/sounds/message-received.wav");
			break;
	}
}

}
