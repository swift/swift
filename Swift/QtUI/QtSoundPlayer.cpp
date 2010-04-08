/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
