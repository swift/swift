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
