/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/SoundPlayer.h>

namespace Swift {
	class DummySoundPlayer : public SoundPlayer {
		public:
			void playSound(SoundEffect sound, const std::string& soundResource) {}
	};
}
