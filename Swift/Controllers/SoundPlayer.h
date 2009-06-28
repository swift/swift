#pragma once

namespace Swift {
	class SoundPlayer {
		public:
			virtual ~SoundPlayer() {};
			enum SoundEffect{MessageReceived};
			virtual void playSound(SoundEffect sound) = 0;
	};
}
