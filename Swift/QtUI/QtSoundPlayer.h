/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/SoundPlayer.h"

#include <QObject>

namespace Swift {
	class QtSoundPlayer : public QObject, public SoundPlayer {
			Q_OBJECT
		public:
			QtSoundPlayer();

			void playSound(SoundEffect sound);
	};
}
