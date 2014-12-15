/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtSoundPlayer.h"

#include <QSound>
#include <iostream>

#include <SwifTools/Application/ApplicationPathProvider.h>
#include <QtSwiftUtil.h>
#include <Swiften/Base/Path.h>

namespace Swift {
	
QtSoundPlayer::QtSoundPlayer(ApplicationPathProvider* applicationPathProvider) : applicationPathProvider(applicationPathProvider) {
}

void QtSoundPlayer::playSound(SoundEffect sound, const std::string& soundResource) {

	switch (sound) {
		case MessageReceived:
			playSound(soundResource.empty() ? "/sounds/message-received.wav" : soundResource);
			break;
	}
}

void QtSoundPlayer::playSound(const std::string& soundResource) {
	boost::filesystem::path resourcePath = applicationPathProvider->getResourcePath(soundResource);
	if (boost::filesystem::exists(resourcePath)) {
		QSound::play(P2QSTRING(pathToString(resourcePath)));
	}
	else if (boost::filesystem::exists(soundResource)) {
		QSound::play(P2QSTRING(soundResource));
	}
	else {
		std::cerr << "Unable to find sound: " << soundResource << std::endl;
	}
}

}
