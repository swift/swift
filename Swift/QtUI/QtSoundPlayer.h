/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/SoundPlayer.h"

#include <QObject>

namespace Swift {
	class ApplicationPathProvider;
	

	class QtSoundPlayer : public QObject, public SoundPlayer {
			Q_OBJECT
		public:
			QtSoundPlayer(ApplicationPathProvider* applicationPathProvider);

			void playSound(SoundEffect sound);

		private:
			void playSound(const std::string& soundResource);

		private:
			ApplicationPathProvider* applicationPathProvider;
	};
}
