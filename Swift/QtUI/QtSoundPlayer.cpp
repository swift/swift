/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSoundPlayer.h"

#include <QSound>
#include <iostream>

#include "SwifTools/Application/ApplicationPathProvider.h"

namespace Swift {
	
QtSoundPlayer::QtSoundPlayer(ApplicationPathProvider* applicationPathProvider) : applicationPathProvider(applicationPathProvider) {
}

void QtSoundPlayer::playSound(SoundEffect sound) {
	switch (sound) {
		case MessageReceived:
			playSound("/sounds/message-received.wav");
			break;
	}
}

void QtSoundPlayer::playSound(const std::string& soundResource) {
	boost::filesystem::path resourcePath = applicationPathProvider->getResourcePath(soundResource);
	if (boost::filesystem::exists(resourcePath)) {
		QSound::play(resourcePath.string().c_str());
	}
	else {
		std::cerr << "Unable to find sound: " << soundResource << std::endl;
	}
}

}
