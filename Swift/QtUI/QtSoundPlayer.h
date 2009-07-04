#pragma once

#include "Swift/Controllers/SoundPlayer.h"

#include <QObject>

namespace Phonon {
	class AudioOutput;
	class MediaObject;
}

namespace Swift {
	class QtSoundPlayer : public QObject, public SoundPlayer {
		Q_OBJECT
		public:
			QtSoundPlayer();
			~QtSoundPlayer();
			void playSound(SoundEffect sound);
		private:
			Phonon::AudioOutput* audioOutput_;
			Phonon::MediaObject* messageReceived_;
		private slots:
			void handleFinished();
	};
}
