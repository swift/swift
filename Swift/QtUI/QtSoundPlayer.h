#pragma once

#include "Swift/Controllers/SoundPlayer.h"


namespace Phonon {
	class AudioOutput;
	class MediaObject;
}

namespace Swift {
	class QtSoundPlayer : public SoundPlayer{
		public:
			QtSoundPlayer();
			~QtSoundPlayer();
			void playSound(SoundEffect sound);
		private:
			Phonon::AudioOutput* audioOutput_;
			Phonon::MediaObject* messageReceived_;
	};
}
