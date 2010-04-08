/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class SoundPlayer {
		public:
			virtual ~SoundPlayer() {};
			enum SoundEffect{MessageReceived};
			virtual void playSound(SoundEffect sound) = 0;
	};
}
